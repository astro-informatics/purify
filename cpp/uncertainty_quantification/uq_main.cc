#include <iostream> 
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/measurement_operator_factory.h"
#include "purify/setup_utils.h"
#include "sopt/objective_functions.h"
#include "sopt/differentiable_func.h"
#include "sopt/non_differentiable_func.h"
#include <stdlib.h>
#include "yaml-cpp/yaml.h"
#include "purify/yaml-parser.h"
#include "purify/setup_utils.h"
#include <vector>
#include <tuple>
#include <string>
#include <sopt/l1_non_diff_function.h>
#include <sopt/tf_non_diff_function.h>
#include <sopt/real_indicator.h>
#include <sopt/l2_differentiable_func.h>
#include <sopt/onnx_differentiable_func.h>


using VectorC = sopt::Vector<std::complex<double>>;

int main(int argc, char **argv)
{
    if(argc != 4)
    {
        std::cout << "purify_UQ should be run using three additional arguments." << std::endl;
        std::cout << "purify_UQ <config_path> <reference_image_path> <surrogate_image_path>" << std::endl;
        std::cout << "<config_path>: path to a .yaml config file specifying details of measurement operator, wavelet operator, observations, and cost functions." << std::endl;
        std::cout << "<reference_image_path>: path to image file (.fits) which was output from running purify on observed data." << std::endl;
        std::cout << "<surrogate_image_path>: path to modified image file (.fits) for feature analysis." << std::endl;
        std::cout << std::endl;
        std::cout << "For more information about the contents of the config file please consult the README." << std::endl;
        return 1;
    }

    // Load and parse the config for parameters 
    const std::string config_path = argv[1];
    const YAML::Node UQ_config = YAML::LoadFile(config_path);

    // Load the Reference and Surrogate images
    const std::string ref_image_path = argv[2];
    const std::string surrogate_image_path = argv[3];
    const auto reference_image = purify::pfitsio::read2d(ref_image_path);
    const VectorC reference_vector = VectorC::Map(reference_image.data(), reference_image.size());
    const auto surrogate_image = purify::pfitsio::read2d(surrogate_image_path);
    const VectorC surrogate_vector = VectorC::Map(surrogate_image.data(), surrogate_image.size());

    const uint imsize_x = reference_image.cols();
    const uint imsize_y = reference_image.rows();

    std::unique_ptr<DifferentiableFunc<t_complex>> f;
    std::unique_ptr<NonDifferentiableFunc<t_complex>> g;

    // Prepare operators and data using purify config
    // If no purify config use basic version for now based on algo_factory test images 
    purify::utilities::vis_params measurement_data;
    std::shared_ptr<sopt::LinearTransform<VectorC>> measurement_operator;
    std::shared_ptr<const sopt::LinearTransform<VectorC>> wavelet_operator;
    std::vector<std::tuple<std::string, t_uint>> const sara{
        std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
        std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
        std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
    if(UQ_config["purify_config_file"])
    {
        YamlParser purify_config = YamlParser(UQ_config["purify_config_file"].as<std::string>());
    
        const auto [mop_algo, wop_algo, using_mpi] = selectOperators(purify_config);
        auto [uv_data, sigma, measurement_op_eigen_vector, image_index, w_stacks] = getInputData(purify_config,
                                                                                                 mop_algo,
                                                                                                 wop_algo,
                                                                                                 using_mpi);
    
        auto [transform, operator_norm] = createMeasurementOperator(purify_config,
                                                                    mop_algo,
                                                                    wop_algo,
                                                                    using_mpi,
                                                                    image_index,
                                                                    w_stacks,
                                                                    uv_data,
                                                                    measurement_op_eigen_vector);
    
        const waveletInfo wavelets = createWaveletOperator(purify_config, wop_algo);
    
        t_real const flux_scale = 1.;
        uv_data.vis = uv_data.vis.array() * uv_data.weights.array() / flux_scale;

        measurement_data = uv_data;
        measurement_operator = transform;
        wavelet_operator = wavelets.transform;

        // set up f and g from config
        switch (purify_config.diffFuncType())
        {
            case purify::diff_func_type::L2Norm:
                f = std::make_unique<sopt::L2DifferentiableFunc<t_complex>>(sigma, *measurement_operator);
                break;
            case purify::diff_func_type::L2Norm_with_CRR:
                f = std::make_unique<sopt::ONNXDifferentiableFunc<t_complex>>(
                    purify_config.CRR_function_model_path(),
                    purify_config.CRR_gradient_model_path(),
                    sigma,
                    purify_config.CRR_mu(),
                    purify_config.CRR_lambda(),
                    *measurement_operator
                );
                break;
        }

        switch (purify_config.nondiffFuncType())
        {
            case purify::nondiff_func_type::L1Norm:
                g = std::make_unique<sopt::algorithm::L1GProximal<t_complex>>();
                break;
            case purify::nondiff_func_type::Denoiser:
                g = std::make_unique<sopt::algorithm::TFGProximal<t_complex>>(
                    purify_config.model_path()
                );
                break;
            case purify::nondiff_func_type::RealIndicator:
                g = std::make_unique<sopt::algorithm::RealIndicator<t_complex>>();
                break;
        }
    }
    else
    {
        const std::string measurements_path = UQ_config["measurements_path"].as<std::string>();
        // Load the images and measurements
        measurement_data = purify::utilities::read_visibility(measurements_path, false);

        // This is the measurement operator used in the test but this should probably be selectable
        measurement_operator = 
        purify::factory::measurement_operator_factory<sopt::Vector<t_complex>>(
            purify::factory::distributed_measurement_operator::serial,
            measurement_data,
            imsize_y,
            imsize_x,
            1,
            1,
            2,
            kernels::kernel_from_string.at("kb"),
            4,
            4);

        wavelet_operator = purify::factory::wavelet_operator_factory<Vector<t_complex>>(
            factory::distributed_wavelet_operator::serial, sara, imsize_y, imsize_x);
    }

    // Set up confidence and objective function params
    double confidence;
    double alpha;
    if((UQ_config["confidence_interval"]) && (UQ_config["alpha"]))
    {
        std::cout << "Config should only contain one of 'confidence_interval' or 'alpha'." << std::endl;
        return 1;
    }
    if(UQ_config["confidence_interval"])
    {
        confidence = UQ_config["confidence_interval"].as<double>();
        alpha = 1-confidence;
    }
    else if(UQ_config["alpha"])
    {
        alpha = UQ_config["alpha"].as<double>();
        confidence = 1 - alpha;
    }
    else
    {
        std::cout << "Config file must contain either 'confidence_interval' or 'alpha' as a parameter." << std::endl;
        return 1;
    }

    const double regulariser_strength = UQ_config["regulariser_strength"].as<double>();


    if((imsize_x != surrogate_image.cols()) || (imsize_y != surrogate_image.rows()))
    {
        std::cout << "Surrogate and reference images have different dimensions. Aborting." << std::endl;
        return 2;
    } 

    
    if( ((*measurement_operator) * reference_vector).size() != measurement_data.vis.size())
    {
        std::cout << "Image size is not compatible with the measurement operator and data provided." << std::endl;
        return 3;
    }

    // Calculate the posterior function for the reference image
    // posterior = likelihood + prior
    // Likelihood = |y - Phi(x)|^2 / sigma^2  (L2 norm)
    // Prior = Sum(Psi^t * |x_i|) * regulariser_strength  (L1 norm)
    auto Posterior = [&measurement_data, measurement_operator, wavelet_operator, regulariser_strength, &f, &g](const VectorC &image) {
      {
        const auto residuals = (*measurement_operator * image) - measurement_data.vis;
        auto A = f->function(image, measurement_data.vis, (*measurement_operator));
        auto B = g->function(image);
        return A + regulariser_strength * B;
      }
    };

    const double reference_posterior = Posterior(reference_vector);
    const double surrogate_posterior = Posterior(surrogate_vector);

    // Threshold for surrogate image posterior to be within confidence limit
    const double N = imsize_x * imsize_y;
    const double tau = std::sqrt( 16 * std::log(3 / alpha) );
    const double threshold = reference_posterior + tau * std::sqrt(N) + N;

    std::cout << "Uncertainty Quantification." << std::endl;
    std::cout << "Reference Log Posterior = " << reference_posterior << std::endl;
    std::cout << "Confidence interval = " << confidence << std::endl;
    std::cout << "Log Posterior threshold = " << threshold << std::endl;
    std::cout << "Surrogate Log Posterior = " << surrogate_posterior << std::endl;
    std::cout << "Surrogate image is "
              << ((surrogate_posterior <= threshold) ? "within the credible interval."
                                                     : "excluded by the credible interval.")
              << std::endl;

    return 0;
}