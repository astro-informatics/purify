#include <array>
#include <memory>
#include <random>
#include <boost/math/special_functions/erf.hpp>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include <sopt/credible_region.h>
#include <sopt/imaging_padmm.h>
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
#include "purify/uvfits.h"
#include "purify/wproj_utilities.h"
using namespace purify;
using namespace purify::notinstalled;

void padmm(const std::string &name, const t_uint &imsizex, const t_uint &imsizey,
           const std::string &kernel, const t_int J, const utilities::vis_params &uv_data,
           const t_real sigma, const std::tuple<bool, t_real> &w_term) {
  std::string const outfile_fits = output_filename(name + "_solution.fits");
  std::string const residual_fits = output_filename(name + "_residual.fits");
  std::string const dirty_image_fits = output_filename(name + "_dirty.fits");
  std::string const psf_image_fits = output_filename(name + "_psf.fits");

  t_real const over_sample = 2;
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> measurements_transform =
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_data, imsizey, imsizex, std::get<1>(w_term), std::get<1>(w_term), over_sample,
          kernels::kernel_from_string.at(kernel), J, J, std::get<0>(w_term));
  t_uint const M = uv_data.size();
  t_uint const N = imsizex * imsizey;
  sopt::wavelets::SARA const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizey, imsizex);
  const Vector<> dimage = (measurements_transform->adjoint() * uv_data.vis).real();
  Matrix<t_complex> point = Matrix<t_complex>::Zero(imsizey, imsizex);
  point(std::floor(imsizey / 2), std::floor(imsizex / 2)) = 1.;
  const Vector<> psf =
      (measurements_transform->adjoint() *
       (*measurements_transform * Vector<t_complex>::Map(point.data(), point.size())))
          .real();
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());
  pfitsio::write2d(Image<t_real>::Map(dimage.data(), imsizey, imsizex), dirty_image_fits);
  pfitsio::write2d(Image<t_real>::Map(psf.data(), imsizey, imsizex), psf_image_fits);
  auto const epsilon = 3 * std::sqrt(2 * uv_data.size()) * sigma;
  auto const gamma = (measurements_transform->adjoint() * uv_data.vis).real().maxCoeff() * 1e-3;
  PURIFY_HIGH_LOG("Using epsilon of {}", epsilon);
#ifdef PURIFY_CImg
  auto const canvas = std::make_shared<CDisplay>(
      cimg::make_display(Vector<t_real>::Zero(2 * imsizex * imsizey), 2 * imsizex, imsizey));
  canvas->resize(true);
  auto const show_image = [&, measurements_transform](const Vector<t_complex> &x) -> bool {
    if (!canvas->is_closed()) {
      const Vector<t_complex> res =
          (measurements_transform->adjoint() * (uv_data.vis - (*measurements_transform * x)));
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
  auto padmm = std::make_shared<sopt::algorithm::ImagingProximalADMM<t_complex>>(uv_data.vis);
  padmm->itermax(500)
      .gamma(gamma)
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
      .nu(1e0)
      .Psi(Psi)
      .Phi(*measurements_transform);

  auto convergence_function = [](const Vector<t_complex> &x) { return true; };
  const std::shared_ptr<t_uint> iter = std::make_shared<t_uint>(0);

  std::weak_ptr<decltype(padmm)::element_type> const padmm_weak(padmm);
  const auto algo_update = [uv_data, imsizex, imsizey, padmm_weak,
                            iter](const Vector<t_complex> &x) -> bool {
    auto padmm = padmm_weak.lock();
    PURIFY_MEDIUM_LOG("Step size γ {}", padmm->gamma());
    *iter = *iter + 1;
    Vector<t_complex> const alpha = padmm->Psi().adjoint() * x;
    // updating parameter
    const t_real new_gamma = alpha.real().cwiseAbs().maxCoeff() * 1e-3;
    PURIFY_MEDIUM_LOG("Step size γ update {}", new_gamma);
    padmm->gamma(((std::abs(padmm->gamma() - new_gamma) > 0.2) and *iter < 200) ? new_gamma
                                                                                : padmm->gamma());

    Vector<t_complex> const residual = padmm->Phi().adjoint() * (uv_data.vis - padmm->Phi() * x);

    pfitsio::write2d(x, imsizey, imsizex, "solution_update.fits");
    pfitsio::write2d(residual, imsizey, imsizex, "residual_update.fits");
    return true;
  };
  auto lambda = [=](Vector<t_complex> const &x) {
    return convergence_function(x)
#ifdef PURIFY_CImg
           and show_image(x)
#endif
           and algo_update(x);
  };
  padmm->is_converged(lambda);
  auto const diagnostic = (*padmm)();
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  pfitsio::write2d(image.real(), outfile_fits);
  Vector<t_complex> residuals = measurements_transform->adjoint() *
                                (uv_data.vis - ((*measurements_transform) * diagnostic.x));
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
  while (!canvas->is_closed()) canvas->wait();
#endif
}

int main(int, char **) {
  sopt::logging::initialize();
  purify::logging::initialize();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  const std::string &name = "real_data";
  const bool w_term = false;
  const t_real cellsize = 20;
  const t_uint imsizex = 1024;
  const t_uint imsizey = 1024;
  const std::string kernel = "kb";
  const std::vector<std::string> inputfiles = {vla_filename("../mwa/uvdump_01.uvfits"),
                                               vla_filename("../mwa/uvdump_02.uvfits")};

  auto uv_data = pfitsio::read_uvfits(inputfiles);
  t_real const sigma = uv_data.weights.norm() / std::sqrt(uv_data.weights.size()) * 0.05;
  uv_data.vis =
      uv_data.vis.array() * uv_data.weights.array() / uv_data.weights.array().cwiseAbs().maxCoeff();
  padmm(name, imsizex, imsizey, kernel, 4, uv_data, sigma, std::make_tuple(w_term, cellsize));
  return 0;
}
