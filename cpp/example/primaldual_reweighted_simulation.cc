#include "purify/config.h"
#include <array>
#include <ctime>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/primal_dual.h>
#include <sopt/positive_quadrant.h>
#include <sopt/relative_variation.h>
#include <sopt/reweighted.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include <sopt/power_method.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

int main(int nargs, char const **args) {
  if(nargs != 8) {
    std::cerr << " Wrong number of arguments! " << '\n';
    return 1;
  }

  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");

  std::string const kernel = args[1];
  t_real const over_sample = std::stod(static_cast<std::string>(args[2]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  t_real const m_over_n = std::stod(static_cast<std::string>(args[4]));
  std::string const test_number = static_cast<std::string>(args[5]);
  t_real const ISNR = std::stod(static_cast<std::string>(args[6]));
  std::string const name = static_cast<std::string>(args[7]);

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
  MeasurementOperator simulate_measurements(uv_data, 8, 8, "kb", sky_model.cols(), sky_model.rows(),
                                            200,
                                            2); // Generating simulated high quality visibilities
  uv_data.vis = simulate_measurements.degrid(sky_model);

  MeasurementOperator measurements(uv_data, J, J, kernel, sky_model.cols(), sky_model.rows(), 200,
                                   over_sample);

  // putting measurement operator in a form that sopt can use
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const nlevels = sara.size();
  
  auto const Psi
      = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());

  // working out value of sigma given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(uv_data.vis, ISNR);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma, 2.0, "chi^");
  PURIFY_MEDIUM_LOG("epsilon is {} ", epsilon);
  
  auto const tau = 0.49;
  PURIFY_MEDIUM_LOG("tau is {} ", tau);

  sopt::Vector<t_complex> rand = sopt::Vector<t_complex>::Random(measurements.imsizey() * measurements.imsizex() * nlevels);
  PURIFY_HIGH_LOG("Setting up power method to calculate sigma values for primal dual");
  auto const pm = sopt::algorithm::PowerMethod<sopt::t_complex>().tolerance(1e-6);

  PURIFY_HIGH_LOG("Calculating sigma1");
  auto const nu1data = pm.AtA(Psi, rand);
  auto const nu1 = nu1data.magnitude.real();
  auto sigma1 = 1e0 / nu1;
  PURIFY_MEDIUM_LOG("sigma1 is {} ", sigma1);
  
  rand = sopt::Vector<t_complex>::Random(measurements.imsizey() * measurements.imsizex() * (over_sample/2));

  PURIFY_HIGH_LOG("Calculating sigma2");  
  auto const nu2data = pm.AtA(measurements_transform, rand);
  auto const nu2 = nu2data.magnitude.real();
  auto sigma2 = 1e0 / nu2;
  PURIFY_MEDIUM_LOG("sigma2 is {} ", sigma2);
  
    PURIFY_HIGH_LOG("Calculating kappa");  
  auto const kappa = ((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3) / nu2;  
  PURIFY_MEDIUM_LOG("kappa is {} ", kappa);


  
  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());



  PURIFY_HIGH_LOG("Starting sopt!");
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  auto const pd = sopt::algorithm::PrimalDual<t_complex>(uv_data.vis)
            .itermax(100)
            .tau(tau)
            .kappa(kappa)
            .sigma1(sigma1)
            .sigma2(sigma2)
            .levels(nlevels)
            .l2ball_epsilon(epsilon)
            .nu(nu2)
            .relative_variation(1e-5)
            .positivity_constraint(true)
            .residual_convergence(epsilon * 1.001)
            .Psi(Psi)
            .Phi(measurements_transform);

  // Timing reconstruction
  auto const min_delta = sigma * std::sqrt(uv_data.vis.size()) / std::sqrt(9 * sky_model.size());
  // Sets weight after each pd iteration.
  // In practice, this means replacing the proximal of the l1 objective function.
  auto const reweighted = sopt::algorithm::reweighted(pd).min_delta(min_delta).is_converged(
      sopt::RelativeVariation<std::complex<t_real>>(1e-5));
  std::clock_t c_start = std::clock();
  auto const diagnostic = reweighted();
  std::clock_t c_end = std::clock();

  Image<t_complex> image = Image<t_complex>::Map(diagnostic.algo.x.data(), measurements.imsizey(),
                                                 measurements.imsizex());

  Vector<t_complex> original = Vector<t_complex>::Map(sky_model.data(), sky_model.size(), 1);
  Image<t_complex> res = sky_model - image;
  Vector<t_complex> residual = Vector<t_complex>::Map(res.data(), image.size(), 1);

  auto snr = 20. * std::log10(original.norm() / residual.norm()); // SNR of reconstruction
  auto total_time = (c_end - c_start) / CLOCKS_PER_SEC; // total time for solver to run in seconds
  // writing snr and time to a file
  std::ofstream out(results);
  out.precision(13);
  out << snr << " " << total_time;
  out.close();

  return 0;
}
