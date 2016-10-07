#include "purify/config.h"
#include <array>
#include <ctime>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

int main(int nargs, char const **args) {
  if(nargs != 9) {
    std::cerr << " Wrong number of arguments! " << '\n';
    return 1;
  }

  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  
  std::string const test_type = args[1];
  std::string const kernel = args[2];
  t_real const over_sample = std::stod(static_cast<std::string>(args[3]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[4])));
  t_real const m_over_n = std::stod(static_cast<std::string>(args[5]));
  std::string const test_number = static_cast<std::string>(args[6]);
  t_real const ISNR = std::stod(static_cast<std::string>(args[7]));
  std::string const name = static_cast<std::string>(args[8]);

  std::string const fitsfile = image_filename(name + ".fits");

  std::string const dirty_image_fits
      = output_filename(name + "_dirty_" + kernel + "_" + test_number + ".fits");
  std::string const results
      = output_filename(name + "_results_" + kernel + "_" + test_number + ".txt");

  auto sky_model = pfitsio::read2d(fitsfile);
  auto sky_model_max = sky_model.array().abs().maxCoeff();
  sky_model = sky_model / sky_model_max;
  t_int const number_of_vis = std::floor(m_over_n * sky_model.size());
  t_real const sigma_m = constant::pi / 3;

  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  PURIFY_MEDIUM_LOG("Number of measurements: {}", uv_data.u.size());

  MeasurementOperator sky_measurements(uv_data, 8, 8, "kb", sky_model.cols(), sky_model.rows(),
                                   100, over_sample);
  uv_data.vis = sky_measurements.degrid(sky_model);
  MeasurementOperator measurements(uv_data, J, J, kernel, sky_model.cols(), sky_model.rows(),
                                   100, over_sample);
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  std::vector<std::tuple<std::string, t_uint>> wavelets;
	
  if (test_type == "clean")
  	wavelets.push_back(std::make_tuple("Dirac" ,3u));
  if (test_type == "ms_clean")
  	wavelets.push_back(std::make_tuple("DB4" ,3u));
  if (test_type == "padmm"){
  	wavelets.push_back(std::make_tuple("Dirac" ,3u));
  	wavelets.push_back(std::make_tuple("DB1" ,3u));
  	wavelets.push_back(std::make_tuple("DB2" ,3u));
  	wavelets.push_back(std::make_tuple("DB3" ,3u));
  	wavelets.push_back(std::make_tuple("DB4" ,3u));
  	wavelets.push_back(std::make_tuple("DB5" ,3u));
  	wavelets.push_back(std::make_tuple("DB6" ,3u));
  	wavelets.push_back(std::make_tuple("DB7" ,3u));
  	wavelets.push_back(std::make_tuple("DB8" ,3u));
  }
  sopt::wavelets::SARA const sara(wavelets.begin(), wavelets.end());
  auto const Psi
      = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());

  // working out value of sigma given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(uv_data.vis, ISNR);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);

  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  // pfitsio::write2d(Image<t_real>::Map(dimage.data(), measurements.imsizey(),
  // measurements.imsizex()), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  auto const purify_gamma
      = (Psi.adjoint() * (measurements_transform.adjoint() * uv_data.vis)).real().maxCoeff() * 1e-3;
  t_int iters = 0;
  auto convergence_function = [&iters](const Vector<t_complex> &x) { iters = iters + 1; return true; };
  PURIFY_HIGH_LOG("Starting sopt!");
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", purify_gamma);
  auto const padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
                         .gamma(purify_gamma)
                         .relative_variation(1e-3)
                         .l2ball_proximal_epsilon(epsilon * 1.001)
                         .tight_frame(false)
                         .l1_proximal_tolerance(1e-2)
                         .l1_proximal_nu(1)
                         .l1_proximal_itermax(50)
                         .l1_proximal_positivity_constraint(true)
                         .l1_proximal_real_constraint(true)
                         .residual_convergence(epsilon * 1.001)
                         .lagrange_update_scale(0.9)
                         .nu(1e0)
                         .Psi(Psi)
			 .itermax(100)
			 .is_converged(convergence_function)
                         .Phi(measurements_transform);
  // Timing reconstructionu

  std::clock_t c_start = std::clock();
  auto const diagnostic = padmm();
  std::clock_t c_end = std::clock();

  // Reading if algo has converged
  t_int converged = 0;
  if (diagnostic.good) {
	converged = 1;
  }
  const t_uint maxiters = iters;

  Image<t_complex> image
      = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey(), measurements.imsizex());

  Vector<t_complex> original = Vector<t_complex>::Map(sky_model.data(), sky_model.size(), 1);
  Image<t_complex> res = sky_model - image;
  Vector<t_complex> residual = Vector<t_complex>::Map(res.data(), image.size(), 1);

  auto snr = 20. * std::log10(original.norm() / residual.norm()); // SNR of reconstruction
  auto total_time = (c_end - c_start) / CLOCKS_PER_SEC; // total time for solver to run in seconds
  // writing snr and time to a file
  std::ofstream out(results);
  out.precision(13);
  out << snr << " " << total_time << " " << converged << " " << maxiters;
  out.close();

  return 0;
}
