#include "purify/types.h"
#include <array>
#include <ctime>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include <sopt/relative_variation.h>
#include <sopt/sdmm.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

int main(int nargs, char const **args) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();
  purify::logging::initialize();

  if (nargs != 6) {
    PURIFY_CRITICAL(" Wrong number of arguments!");
    return 1;
  }

  std::string const fitsfile = image_filename("M31.fits");

  std::string const kernel = args[1];
  t_real const over_sample = std::stod(static_cast<std::string>(args[2]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  t_real const m_over_n = std::stod(static_cast<std::string>(args[4]));
  std::string const test_number = static_cast<std::string>(args[5]);

  std::string const dirty_image_fits =
      output_filename("M31_dirty_" + kernel + "_" + test_number + ".fits");
  std::string const results = output_filename("M31_results_" + kernel + "_" + test_number + ".txt");

  auto sky_model = pfitsio::read2d(fitsfile);
  auto sky_model_max = sky_model.array().abs().maxCoeff();
  sky_model = sky_model / sky_model_max;
  t_int const number_of_vis = std::floor(m_over_n * sky_model.size());
  t_real const sigma_m = constant::pi / 3;

  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = utilities::vis_units::radians;
  PURIFY_MEDIUM_LOG("Number of measurements: {}", uv_data.u.size());
  auto simulate_measurements = *measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data.u, uv_data.v, uv_data.w, uv_data.weights, sky_model.cols(), sky_model.rows(), 2, 100,
      1e-4, kernels::kernel_from_string.at("kb"), 8, 8);
  uv_data.vis = simulate_measurements * sky_model;

  auto measurements_transform = *measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data.u, uv_data.v, uv_data.w, uv_data.weights, sky_model.cols(), sky_model.rows(),
      over_sample, 100, 1e-4, kernels::kernel_from_string.at(kernel), J, J);

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, sky_model.rows(), sky_model.cols());

  // working out value of sigma given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(uv_data.vis, 30.);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(uv_data.vis, 0., sigma);

  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), sky_model.rows(), sky_model.cols()),
                   dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis.size(), sigma);
  PURIFY_MEDIUM_LOG("Using epsilon of {}", epsilon);
  auto const sdmm =
      sopt::algorithm::SDMM<t_complex>()
          .itermax(500)
          .gamma((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3)
          .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
          .conjugate_gradient(100, 1e-3)
          .append(
              sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -uv_data.vis),
              measurements_transform)
          .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
          .append(sopt::proximal::positive_quadrant<t_complex>);
  // Timing reconstruction
  Vector<t_complex> result;
  std::clock_t c_start = std::clock();
  auto const diagnostic = sdmm(result, initial_estimate);
  if (not diagnostic.good) PURIFY_HIGH_LOG("SDMM did no converge");
  std::clock_t c_end = std::clock();
  Image<t_complex> image = Image<t_complex>::Map(result.data(), sky_model.rows(), sky_model.cols());
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;

  Vector<t_complex> original = Vector<t_complex>::Map(sky_model.data(), sky_model.size(), 1);
  Image<t_complex> res = sky_model - image;
  Vector<t_complex> residual = Vector<t_complex>::Map(res.data(), image.size(), 1);

  auto snr = 20. * std::log10(original.norm() / residual.norm());  // SNR of reconstruction
  auto total_time = (c_end - c_start) / CLOCKS_PER_SEC;  // total time for solver to run in seconds
  // writing snr and time to a file
  std::ofstream out(results);
  out.precision(13);
  out << snr << " " << total_time;
  out.close();

  return 0;
}
