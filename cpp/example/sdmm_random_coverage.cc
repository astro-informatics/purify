#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/relative_variation.h>
#include <sopt/sdmm.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

int main(int, char **) {
  using namespace purify;
  using namespace purify::notinstalled;
  sopt::logging::initialize();

  std::string const fitsfile = image_filename("M31.fits");
  std::string const inputfile = output_filename("M31_input.fits");
  std::string const outfile = output_filename("M31.tiff");
  std::string const outfile_fits = output_filename("M31_solution.fits");
  std::string const residual_fits = output_filename("M31_residual.fits");
  std::string const dirty_image = output_filename("M31_dirty.tiff");
  std::string const dirty_image_fits = output_filename("M31_dirty.fits");
  std::string const output_vis_file = output_filename("M31_Random_coverage.vis");

  t_real const over_sample = 2;
  auto M31 = pfitsio::read2d(fitsfile);
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);
  // Following same formula in matlab example
  t_real const sigma_m = constant::pi / 3;
  // t_int const number_of_vis = std::floor(p * rho * M31.size());
  t_int const number_of_vis = 1e4;
  // Generating random uv(w) coverage
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  utilities::write_visibility(uv_data, output_vis_file);
  std::cout << "Number of measurements / number of pixels: " << uv_data.u.size() * 1. / M31.size()
            << '\n';
  // uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
  MeasurementOperator measurements(uv_data, 4, 4, "kb", M31.cols(), M31.rows(), 20, over_sample);
  // putting measurement operator in a form that sopt can use
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi
      = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());

  std::mt19937_64 mersenne;
  Vector<t_complex> const y0
      = (measurements_transform * Vector<t_complex>::Map(M31.data(), M31.size()));
  // working out value of signal given SNR of 30
  t_real sigma = utilities::SNR_to_standard_deviation(y0, 30.);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  sopt::utilities::write_tiff(
      Image<t_real>::Map(dimage.data(), measurements.imsizey(), measurements.imsizex()),
      dirty_image);
  pfitsio::write2d(
      Image<t_real>::Map(dimage.data(), measurements.imsizey(), measurements.imsizex()),
      dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis, sigma);
  std::printf("Using epsilon of %f \n", epsilon);
  std::cout << "Starting sopt" << '\n';
  auto const sdmm
      = sopt::algorithm::SDMM<t_complex>()
            .itermax(500)
            .gamma((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3)
            .is_converged(sopt::RelativeVariation<t_complex>(1e-3))
            .conjugate_gradient(100, 1e-3)
            .append(
                sopt::proximal::translate(sopt::proximal::L2Ball<t_complex>(epsilon), -uv_data.vis),
                measurements_transform)
            .append(sopt::proximal::l1_norm<t_complex>, Psi.adjoint(), Psi)
            .append(sopt::proximal::positive_quadrant<t_complex>);
  Vector<t_complex> result;
  auto const diagonstic = sdmm(result);
  Image<t_complex> image
      = Image<t_complex>::Map(result.data(), measurements.imsizey(), measurements.imsizex());
  t_real const max_val_final = image.array().abs().maxCoeff();
  image = image / max_val_final;
  sopt::utilities::write_tiff(image.real(), outfile);
  pfitsio::write2d(image.real(), outfile_fits);
  Image<t_complex> residual = measurements.grid(y0 - measurements.degrid(image));
  pfitsio::write2d(residual.real(), residual_fits);
  return 0;
}
