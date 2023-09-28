#include <iostream> 
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/measurement_operator_factory.h"
#include "sopt/objective_functions.h"
#include <stdlib.h>
#include "yaml-cpp/yaml.h"
#include "purify/yaml-parser.h"

using VectorC = sopt::Vector<std::complex<double>>;

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << "purify_UQ should be run using a single additional argument, which is the path to the config (yaml) file." << std::endl;
        std::cout << "purify_UQ <config_path>" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information about the contents of the config file please consult the README." << std::endl;
        return 1;
    }

    // Load and parse the config for parameters 
    const std::string config_path = argv[1];
    const YAML::Node UQ_config = YAML::LoadFile(config_path);

    const std::string measurements_path = UQ_config["measurements_path"].as<std::string>();
    const std::string ref_image_path = UQ_config["reference_image_path"].as<std::string>();
    const std::string surrogate_image_path = UQ_config["surrogate_image_path"].as<std::string>();
    const std::string purify_config_path = UQ_config["purify_config_path"].as<std::string>();
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
    const double sigma = UQ_config["sigma"].as<double>();
    const double gamma = UQ_config["gamma"].as<double>();
    purify::YamlParser purify_config = purify::YamlParser(purify_config_path);

    // Load the images and measurements
    const purify::utilities::vis_params measurement_data = purify::utilities::read_visibility(measurements_path, false);

    const auto reference_image = purify::pfitsio::read2d(ref_image_path);
    const VectorC reference_vector = VectorC::Map(reference_image.data(), reference_image.size());
    const auto surrogate_image = purify::pfitsio::read2d(surrogate_image_path);
    const VectorC surrogate_vector = VectorC::Map(surrogate_image.data(), surrogate_image.size());


    const uint imsize_x = reference_image.cols();
    const uint imsize_y = reference_image.rows();

    if((imsize_x != surrogate_image.cols()) || (imsize_y != surrogate_image.rows()))
    {
        std::cout << "Surrogate and reference images have different dimensions. Aborting." << std::endl;
        return 2;
    } 

    // This is the measurement operator used in the test but this should probably be selectable
    auto const measurement_operator = 
        purify::factory::measurement_operator_factory<sopt::Vector<std::complex<double>>>(
            purify::factory::distributed_measurement_operator::serial,
            measurement_data,
            imsize_y,
            imsize_x,
            purify_config.cellsizey(),
            purify_config.cellsizex(),
            purify_config.oversampling(),
            purify::kernels::kernel_from_string.at(purify_config.kernel()),
            purify_config.sim_J()
        );

    if( ((*measurement_operator) * reference_vector).size() != measurement_data.vis.size())
    {
        std::cout << "Image size is not compatible with the measurement operator and data provided." << std::endl;
        return 3;
    }

    // Calculate the posterior function for the reference image
    // posterior = likelihood + prior
    // Likelihood = |y - Phi(x)|^2 / sigma^2  (L2 norm)
    // Prior = Sum(|x_i|) * gamma  (L1 norm)
    auto Posterior = [&measurement_data, measurement_operator, sigma, gamma](const VectorC &image) {
      {
        const auto residuals = (*measurement_operator * image) - measurement_data.vis;
        return residuals.squaredNorm() / (2 * sigma * sigma) + image.cwiseAbs().sum() * gamma;
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