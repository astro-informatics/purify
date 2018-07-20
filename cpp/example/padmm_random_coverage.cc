#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include <sopt/credible_region.h>
#include <sopt/imaging_padmm.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#ifdef PURIFY_GPU
#include "purify/operators_gpu.h"
#endif
#include "purify/cimg.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
using namespace purify;
using namespace purify::notinstalled;

void padmm(const std::string &name, const Image<t_complex> &M31, const std::string & kernel,
           const t_int J, const utilities::vis_params &uv_data, const t_real sigma,
           const std::tuple<bool, t_real> &w_term) {

  std::string const outfile = output_filename(name + "_" + kernel + ".tiff");
  std::string const outfile_fits = output_filename(name + "_" + kernel + "_solution.fits");
  std::string const residual_fits = output_filename(name + "_" + kernel + "_residual.fits");
  std::string const dirty_image = output_filename(name + "_" + kernel + "_dirty.tiff");
  std::string const dirty_image_fits = output_filename(name + "_" + kernel + "_dirty.fits");

  t_real const over_sample = 2;
  t_uint const imsizey = M31.rows();
  t_uint const imsizex = M31.cols();
  utilities::write_visibility(uv_data, output_filename("input_data.vis"));
#ifndef PURIFY_GPU
  auto const measurements_transform
      = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 1000,
          0.0001, kernels::kernel_from_string.at(kernel), J, J, std::get<0>(w_term));
#else
  af::setDevice(0);
  auto const measurements_transform = gpu::measurementoperator::init_degrid_operator_2d(
      uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample, 1000, 0.0001,
      kernels::kernel_from_string.at(kernel), J, J, std::get<0>(w_term));
#endif
  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
  Vector<> dimage = (measurements_transform->adjoint() * uv_data.vis).real();
  assert(dimage.size() == M31.size());
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), imsizey, imsizex), dirty_image);
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), imsizey, imsizex), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis.size(), sigma);
  PURIFY_HIGH_LOG("Using epsilon of {}", epsilon);
#ifdef PURIFY_CImg
  auto const canvas
      = std::make_shared<CDisplay>(cimg::make_display(Vector<t_real>::Zero(1024 * 512), 1024, 512));
  canvas->resize(true);
  auto const show_image = [&, measurements_transform](const Vector<t_complex> &x) -> bool {
    if(!canvas->is_closed()) {
      const Vector<t_complex> res
          = (measurements_transform->adjoint() * (uv_data.vis - (*measurements_transform * x)));
      const auto img1 = cimg::make_image(x.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512);
      const auto img2 = cimg::make_image(res.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512);
      const auto results = CImageList<t_real>(img1, img2);
      canvas->display(results);
      canvas->resize(true);
    }
    return true;
  };
#endif
  auto const padmm
      = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
            .itermax(500)
            .gamma((Psi.adjoint() * (measurements_transform->adjoint() * uv_data.vis)).cwiseAbs().maxCoeff() * 1e-3)
            .relative_variation(1e-3)
            .l2ball_proximal_epsilon(epsilon)
            .tight_frame(false)
            .l1_proximal_tolerance(1e-2)
            .l1_proximal_nu(1.)
            .l1_proximal_itermax(50)
            .l1_proximal_positivity_constraint(true)
            .l1_proximal_real_constraint(true)
            .residual_convergence(epsilon)
            .lagrange_update_scale(0.9)
#ifdef PURIFY_CImg
            .is_converged(show_image)
#endif
            .nu(1e0)
            .Psi(Psi)
            .Phi(*measurements_transform);

  auto const diagnostic = padmm();
  assert(diagnostic.x.size() == M31.size());
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  pfitsio::write2d(image.real(), outfile_fits);
  Vector<t_complex> residuals = measurements_transform->adjoint()
                                * (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  pfitsio::write2d(residual_image.real(), residual_fits);
#ifdef PURIFY_CImg
  const auto results = CImageList<t_real>(
      cimg::make_image(diagnostic.x.real().eval(), imsizey, imsizex).get_resize(512, 512),
      cimg::make_image(residuals.real().eval(), imsizey, imsizex).get_resize(512, 512));
  canvas->display(results);
  cimg::make_image(residuals.real().eval(), imsizey, imsizex)
      .histogram(256)
      .display_graph("Residual Histogram", 2);
  while(!canvas->is_closed())
    canvas->wait();
#endif
}

int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
 // sopt::logging::set_level("debug");
//  purify::logging::set_level("debug");
  const std::string &name = "M31";
  const t_real FoV = 1;      // deg
  const t_real max_w = 100.; // lambda
  const t_real snr = 10;
  const bool w_term = false;
  const std::string kernel = "kb";
  std::string const fitsfile = image_filename(name + ".fits");
  auto M31 = pfitsio::read2d(fitsfile);
  const t_real cellsize = 1;//FoV / M31.cols() * 60. * 60.;
  std::string const inputfile = output_filename(name + "_" + "input.fits");

  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);

  t_int const number_of_pxiels = M31.size();
  t_int const number_of_vis = std::floor(number_of_pxiels * 0.2);
  // Generating random uv(w) coverage
  /*
  t_real const sigma_m = constant::pi / 3;
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
  uv_data.units = utilities::vis_units::radians;
  PURIFY_MEDIUM_LOG("Number of measurements / number of pixels: {}",
                    uv_data.u.size() * 1. / number_of_pxiels);

#ifndef PURIFY_GPU
  auto const sky_measurements = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_data, M31.rows(), M31.cols(), cellsize, cellsize, 2, 1000, 0.0001, kernels::kernel_from_string.at("kb"), 8,
      8, w_term);
#else
  auto const sky_measurements = gpu::measurementoperator::init_degrid_operator_2d(
      uv_data, M31.rows(), M31.cols(), cellsize, cellsize, 2, 1000, 0.0001, kernels::kernel_from_string.at("kb"), 8,
      8, w_term);
#endif
  uv_data.vis = (*sky_measurements) * Image<t_complex>::Map(M31.data(), M31.size(), 1);
  Vector<t_complex> const y0 = uv_data.vis;
  // working out value of signal given SNR of 30
  t_real const sigma = utilities::SNR_to_standard_deviation(y0, snr);

  std::cout << std::setprecision(13) << sigma << std::endl;
  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  // padmm(name + "30", M31, "box", 1, uv_data, sigma, std::make_tuple(w_term, cellsize));
  */
  
  const std::string &test_dir = "expected/padmm_serial/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");
  auto uv_data = utilities::read_visibility(input_data_path, false);
  uv_data.units = utilities::vis_units::radians;
  t_real const sigma = 0.02378738741225;
  padmm(name + "10", M31, kernel, 4, uv_data, sigma,
        std::make_tuple(w_term, cellsize));
  return 0;
}
