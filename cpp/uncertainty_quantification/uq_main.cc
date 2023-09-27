#include <iostream> 
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/measurement_operator_factory.h"
#include "sopt/objective_functions.h"
#include <stdlib.h>

using VectorC = sopt::Vector<std::complex<double>>;

int main(int argc, char **argv)
{
    if(argc != 7)
    {
        std::cout << "Please provide the following six arguments: " << std::endl;
        std::cout << "Path for measurement data." << std::endl;
        std::cout << "Path for reference image (.fits file)." << std::endl;
        std::cout << "Path for surrogate iamge (.fits file)." << std::endl;
        std::cout << "Confidence interval." << std::endl;
        std::cout << "sigma (Gaussian Likelihood parameter)." << std::endl;
        std::cout << "gamma (scaling of L1-norm prior)." << std::endl;
        return 1;
    }

    const std::string measurements_path = argv[1];
    const std::string ref_image_path = argv[2];
    const std::string surrogate_image_path = argv[3];
    const double confidence = strtod(argv[4], nullptr);
    const double alpha = 1 - confidence;
    const double sigma = strtod(argv[5], nullptr);
    const double gamma = strtod(argv[6], nullptr);
    
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
            1,
            1,
            2,
            purify::kernels::kernel_from_string.at("kb"),
            4,
            4
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