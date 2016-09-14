#include <array>
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
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include "purify/logging.h"

using namespace purify;
using namespace purify::notinstalled;

void padmm(const std::string & name, const Image<t_complex> & M31, const std::string & kernel, const t_int J, const utilities::vis_params & uv_data, const t_real sigma){
  std::string const outfile = output_filename(name + "_" + kernel + ".tiff");
  std::string const outfile_fits = output_filename(name +  "_" + kernel + "_solution.fits");
  std::string const residual_fits = output_filename(name +  "_" + kernel + "_residual.fits");
  std::string const dirty_image = output_filename(name +  "_" + kernel + "_dirty.tiff");
  std::string const dirty_image_fits = output_filename(name +  "_" + kernel + "_dirty.fits");


  t_real const over_sample = 2;
  MeasurementOperator measurements(uv_data, J, J, kernel, M31.cols(), M31.rows(), 100, over_sample);
  auto measurements_transform = linear_transform(measurements, uv_data.vis.size());

  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi
      = sopt::linear_transform<t_complex>(sara, measurements.imsizey(), measurements.imsizex());

  Vector<> dimage = (measurements_transform.adjoint() * uv_data.vis).real();
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
  auto const padmm
      = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
            .itermax(100)
            .gamma((measurements_transform.adjoint() * uv_data.vis).real().maxCoeff() * 1e-3)
            .relative_variation(1e-3)
            .l2ball_proximal_epsilon(epsilon)
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
            .Phi(measurements_transform);

  auto const diagnostic = padmm();
  assert(diagnostic.x.size() == M31.size());
  Image<t_complex> image
      = Image<t_complex>::Map(diagnostic.x.data(), measurements.imsizey(), measurements.imsizex());
  pfitsio::write2d(image.real(), outfile_fits);
  Image<t_complex> residual = measurements.grid(uv_data.vis - measurements.degrid(image));
  pfitsio::write2d(residual.real(), residual_fits);
};


int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  const std::string & name = "30dor_256";
  const t_real snr = 30;
  std::string const fitsfile = image_filename(name + ".fits");
  auto M31 = pfitsio::read2d(fitsfile);
  std::string const inputfile = output_filename(name + "_" + "input.fits");
  
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);
  
  t_int const number_of_pxiels = M31.size();
  t_int const number_of_vis = std::floor( number_of_pxiels * 2.);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";
  std::cout << "Number of measurements / number of pixels: " << uv_data.u.size() * 1. / number_of_pxiels
            << '\n';
  // uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
  MeasurementOperator sky_measurements(uv_data, 8, 8, "kb", M31.cols(), M31.rows(), 100, 2);
  uv_data.vis = sky_measurements.degrid(M31);
  Vector<t_complex> const y0 = uv_data.vis;
  // working out value of signal given SNR of 30
  t_real const sigma = utilities::SNR_to_standard_deviation(y0, snr);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  padmm(name + "30", M31, "box", 1, uv_data, sigma);
  padmm(name + "30", M31, "kb", 4, uv_data, sigma);
  return 0;
}
