#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/credible_region.h>
#include <sopt/imaging_forward_backward.h>
#include <sopt/objective_functions.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/compact_operators.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#ifdef PURIFY_ARRAYFIRE
#include "purify/compact_operators_gpu.h"
#include "purify/operators_gpu.h"
#endif
#include "purify/cimg.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
using namespace purify;
using namespace purify::notinstalled;

void forward_backward(const std::string &name, const Image<t_complex> &M31,
                      const std::string &kernel, const t_int J,
                      const utilities::vis_params &uv_data, const t_real sigma,
                      const std::tuple<bool, t_real> &w_term) {
  std::string const outfile = output_filename(name + "_" + kernel + ".tiff");
  std::string const outfile_fits = output_filename(name + "_" + kernel + "_solution.fits");
  std::string const residual_fits = output_filename(name + "_" + kernel + "_residual.fits");
  std::string const dirty_image = output_filename(name + "_" + kernel + "_dirty.tiff");
  std::string const dirty_image_fits = output_filename(name + "_" + kernel + "_dirty.fits");

  t_real const over_sample = 2;
  t_uint const imsizey = M31.rows();
  t_uint const imsizex = M31.cols();

#if PURIFY_GPU == 1
  af::setDevice(0);
  auto const measurements_transform = gpu::operators::init_grid_degrid_operator_2d(
      uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 500, 0.0001,
      kernel, J, J, std::get<0>(w_term));
  auto const measurements_dirty_map = gpu::measurementoperator::init_degrid_operator_2d(
      uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 500, 0.0001,
      kernel, 2 * J, 2 * J, std::get<0>(w_term));

#else
  auto const measurements_transform = operators::init_grid_degrid_operator_2d<Vector<t_complex>>(
      uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 500, 0.0001,
      kernel, J, J, operators::fftw_plan::measure, std::get<0>(w_term));
  auto const measurements_dirty_map
      = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 500,
          0.0001, kernel, J, J, operators::fftw_plan::measure, std::get<0>(w_term));
#endif
  const Vector<t_complex> dimage = measurements_dirty_map->adjoint() * uv_data.vis;
  pfitsio::write2d(Image<t_complex>::Map(dimage.data(), imsizey, imsizex).real(), dirty_image_fits);
  PURIFY_HIGH_LOG("Creating wavelet operator");
  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const wavelet = sopt::wavelets::factory("DB4", 4);
  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
  auto const dimage_wavelet = Psi.adjoint() * dimage;
  sopt::t_real const mu = 1e-5 * dimage.size();
  sopt::t_real const beta = 0.5 / static_cast<sopt::t_real>(dimage.size());
  PURIFY_HIGH_LOG("Wavelet Coefficients: {}", static_cast<t_int>(dimage_wavelet.size()));
#ifdef PURIFY_CImg
  auto const canvas
      = std::make_shared<CDisplay>(cimg::make_display(Vector<t_real>::Zero(1024 * 512), 1024, 512));
  auto const show_image = [&](const Vector<t_complex> &x) -> bool {
    if(!canvas->is_closed()) {
      Vector<t_complex> res = x;
      measurements_transform(res, x);
      res = dimage - res;
      const auto img1 = cimg::make_image(x.real().eval(), imsizey, imsizex).get_resize(512, 512);
      const auto img2 = cimg::make_image(res.real().eval(), imsizey, imsizex).get_resize(512, 512);
      const auto results = CImageList<t_real>(img1, img2);
      canvas->display(results);
    }
    return true;
  };
#endif
  auto const fb = sopt::algorithm::ImagingForwardBackward<t_complex>(dimage)
                      .itermax(500)
                      .mu(mu)
                      .sigma(sigma)
                      .beta(beta)
                      .relative_variation(1e-3)
                      .tight_frame(false)
                      .residual_tolerance(0)
                      .l1_proximal_tolerance(1e-2)
                      .l1_proximal_nu(1)
                      .l1_proximal_itermax(50)
                      .l1_proximal_positivity_constraint(true)
                      .l1_proximal_real_constraint(true)
#ifdef PURIFY_CImg
                      .is_converged(show_image)
#endif
                      .Psi(Psi)
                      .PhiTPhi(measurements_transform);
  auto const diagnostic = fb();
  assert(diagnostic.x.size() == M31.size());
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  pfitsio::write2d(image.real(), outfile_fits);
  Vector<t_complex> residuals;
  measurements_transform(residuals, diagnostic.x);
  residuals = dimage - residuals;
  const Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  pfitsio::write2d(residual_image.real(), residual_fits);
#ifdef PURIFY_CImg
  const auto results = CImageList<t_real>(
      cimg::make_image(diagnostic.x.real().eval(), imsizey, imsizex).get_resize(512, 512),
      cimg::make_image(residuals.real().eval(), imsizey, imsizex).get_resize(512, 512));
  canvas->display(results);
  canvas->resize(1024, 512, true);
  cimg::make_image(residuals.real().eval(), imsizey, imsizex)
      .histogram(256)
      .display_graph("Residual Histogram", 2);
#endif
  Image<t_real> lower_error, upper_error, mean_solution;
  const t_real alpha = 0.99;
  const std::tuple<t_uint, t_uint> grid_pixel_size = std::make_tuple(imsizey / 4, imsizex / 4);
  auto const obj_function = sopt::objective_functions::unconstrained_l1_regularisation(
      mu, sigma, uv_data.vis, *measurements_dirty_map, Psi);
  std::tie(lower_error, mean_solution, upper_error)
      = sopt::credible_region::credible_interval<Vector<t_complex>, t_real>(
          diagnostic.x, imsizey, imsizex, grid_pixel_size, obj_function, alpha);
#ifdef PURIFY_CImg
  const auto credible_interval
      = CImageList<t_real>(cimg::make_image(lower_error).get_resize(512, 512),
                           cimg::make_image(mean_solution).get_resize(512, 512),
                           cimg::make_image(upper_error).get_resize(512, 512));
  auto const new_canvas
      = std::make_shared<CDisplay>(cimg::make_display(Vector<t_real>::Zero(1536 * 512), 1536, 512));
  new_canvas->display(credible_interval);
  new_canvas->resize(1536, 512, true);
  while(!canvas->is_closed() or !new_canvas->is_closed())
    canvas->wait();
#endif
}

int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  const std::string &name = "M31";
  const t_real FoV = 1;      // deg
  const t_real max_w = 100.; // lambda
  const t_real snr = 30;
  const bool w_term = false;
  std::string const fitsfile = image_filename(name + ".fits");
  auto M31 = pfitsio::read2d(fitsfile);
  // const t_real cellsize = FoV / M31.cols() * 60. * 60.;
  const t_real cellsize = 1;
  std::string const inputfile = output_filename(name + "_" + "input.fits");

  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);

  t_int const number_of_pixels = M31.size();
  t_int const number_of_vis = std::floor(number_of_pixels * 0.5);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
  uv_data.units = "radians";
  PURIFY_MEDIUM_LOG("Number of measurements / number of pixels: {}",
                    uv_data.u.size() * 1. / number_of_pixels);
#if PURIFY_GPU == 1
  auto const sky_measurements = gpu::measurementoperator::init_degrid_operator_2d(
      uv_data, M31.rows(), M31.cols(), cellsize, cellsize, 2, 500, 0.0001, "kb", 8, 8, w_term);
#else
  auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data, M31.rows(), M31.cols(), cellsize, cellsize, 2, 500, 0.0001, "kb", 8, 8,
      operators::fftw_plan::measure, w_term);
#endif
  uv_data.vis = (*sky_measurements) * Vector<t_complex>::Map(M31.data(), M31.size());
  Vector<t_complex> const y0 = uv_data.vis;
  // working out value of signal given SNR of 30
  t_real const sigma = utilities::SNR_to_standard_deviation(y0, snr);
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  // padmm(name + "30", M31, "box", 1, uv_data, sigma, std::make_tuple(w_term, cellsize));
  forward_backward(name + "30", M31, "kb", 4, uv_data, sigma, std::make_tuple(w_term, cellsize));
  return 0;
}
