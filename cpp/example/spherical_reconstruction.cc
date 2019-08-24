#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/spherical_resample.h"
#include <sopt/credible_region.h>
#include <sopt/imaging_padmm.h>
#include <sopt/power_method.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>
#ifdef PURIFY_GPU
#include "purify/operators_gpu.h"
#endif
#include "purify/types.h"
#include "purify/cimg.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
using namespace purify;
using namespace purify::notinstalled;

void padmm(
    const std::string &name, const Image<t_complex> &all_sky_image,
    const utilities::vis_params &uv_data, const t_real sigma,
    const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> &measurements_transform) {
  std::string const outfile = output_filename(name + ".tiff");
  std::string const outfile_fits = output_filename(name + "_solution.fits");
  std::string const residual_fits = output_filename(name + "_residual.fits");
  std::string const dirty_image = output_filename(name + "_dirty.tiff");
  std::string const dirty_image_fits = output_filename(name + "_dirty.fits");
  t_uint const imsizey = all_sky_image.rows();
  t_uint const imsizex = all_sky_image.cols();

  sopt::wavelets::SARA const sara{std::make_tuple("Dirac", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
  Vector<> dimage = (measurements_transform->adjoint() * uv_data.vis).real();
  assert(dimage.size() == all_sky_image.size());
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  sopt::utilities::write_tiff(Image<t_real>::Map(dimage.data(), imsizey, imsizex), dirty_image);
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), imsizey, imsizex), dirty_image_fits);

  auto const epsilon = utilities::calculate_l2_radius(uv_data.vis.size(), sigma);
  PURIFY_HIGH_LOG("Using epsilon of {}", epsilon);
#ifdef PURIFY_CImg
  auto const canvas =
      std::make_shared<CDisplay>(cimg::make_display(Vector<t_real>::Zero(1024 * 512), 1024, 512));
  canvas->resize(true);
  auto const show_image = [&, measurements_transform](const Vector<t_complex> &x) -> bool {
    if (!canvas->is_closed()) {
      const Vector<t_complex> res =
          (measurements_transform->adjoint() * (uv_data.vis - (*measurements_transform * x)));
      const auto img1 = cimg::make_image(x.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512/2);
      const auto img2 = cimg::make_image(res.real().eval(), imsizey, imsizex)
                            .get_normalize(0, 1)
                            .get_resize(512, 512/2);
      const auto results = CImageList<t_real>(img1, img2);
      canvas->display(results);
      canvas->resize(true);
    }
    return true;
  };
#endif
  auto const padmm =
      sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
          .itermax(500)
          .gamma((Psi.adjoint() * (measurements_transform->adjoint() * uv_data.vis).eval())
                     .cwiseAbs()
                     .maxCoeff() *
                 1e-3)
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
  assert(diagnostic.x.size() == all_sky_image.size());
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  pfitsio::write2d(image.real(), outfile_fits);
  Vector<t_complex> residuals = measurements_transform->adjoint() *
                                (uv_data.vis - ((*measurements_transform) * diagnostic.x));
  Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
  pfitsio::write2d(residual_image.real(), residual_fits);
#ifdef PURIFY_CImg
  const auto results = CImageList<t_real>(
      cimg::make_image(diagnostic.x.real().eval(), imsizey, imsizex).get_resize(512, 512 / 2),
      cimg::make_image(residuals.real().eval(), imsizey, imsizex).get_resize(512, 512 / 2));
  canvas->display(results);
  cimg::make_image(residuals.real().eval(), imsizey, imsizex)
      .histogram(256)
      .display_graph("Residual Histogram", 2);
  while (!canvas->is_closed()) canvas->wait();
#endif
}

int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
  // sopt::logging::set_level("debug");
  //  purify::logging::set_level("debug");
  const std::string &name = "allsky400MHz";
  const t_real L = 0.5;
  const t_real max_w = 0.;  // lambda
  const t_real snr = 30;
  std::string const fitsfile = image_filename(name + ".fits");
  const auto all_sky_image = pfitsio::read2d(fitsfile);
  std::string const inputfile = output_filename(name + "_" + "input.fits");

  pfitsio::write2d(all_sky_image.real(), inputfile);
  const t_real theta_0 = 180. * constant::pi / 180.;
  const t_real phi_0 = 120. * constant::pi / 180.;

  t_int const number_of_pxiels = all_sky_image.size();
  t_int const number_of_vis = 1e6;
  // Generating random uv(w) coverage
  t_real const sigma_m = 1000. / 4. / 3. / L;
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m, max_w);
  uv_data.units = utilities::vis_units::lambda;
  PURIFY_MEDIUM_LOG("Number of measurements / number of pixels: {}",
                    uv_data.u.size() * 1. / number_of_pxiels);
  t_uint const imsizey = all_sky_image.rows();
  t_uint const imsizex = all_sky_image.cols();
  const t_int num_theta = all_sky_image.rows();
  const t_int num_phi = all_sky_image.cols();

  const t_int number_of_samples = num_theta * num_phi;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_int Jw = 14;
  const t_real oversample_ratio_image_domain = 2;
  const t_real oversample_ratio = 2;
  const bool uvw_stacking = true;
  const kernels::kernel kernel = kernels::kernel::kb;
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;

  const auto phi = [num_phi, num_theta](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> const sky_measurements =
      std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          spherical_resample::measurement_operator::nonplanar_degrid_wproj_operator<
              Vector<t_complex>, std::function<t_real(t_int)>>(
              number_of_samples, theta_0, phi_0, theta, phi, uv_data, oversample_ratio,
              oversample_ratio_image_domain, kernel, Ju, Jw, Jl, Jm, ft_plan, uvw_stacking, L, 1e-6,
              1e-6),
          1000, 1e-4, Vector<t_complex>::Random(imsizex * imsizey).eval()));
  uv_data.vis =
      (*sky_measurements) * Image<t_complex>::Map(all_sky_image.data(), all_sky_image.size(), 1);
  Vector<t_complex> const y0 = uv_data.vis;
  // working out value of signal given SNR of 30
  t_real const sigma = utilities::SNR_to_standard_deviation(y0, snr);

  // adding noise to visibilities
  uv_data.vis = utilities::add_noise(y0, 0., sigma);
  padmm(name + "30", all_sky_image, uv_data, sigma, sky_measurements);
  return 0;
}
