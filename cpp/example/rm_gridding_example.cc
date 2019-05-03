#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/rm_operators.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>

#include <sopt/credible_region.h>
#include <sopt/imaging_padmm.h>
#include <sopt/power_method.h>
#include <sopt/relative_variation.h>
#include <sopt/utilities.h>
#include <sopt/wavelets.h>
#include <sopt/wavelets/sara.h>

using namespace purify;

namespace {
void run_admm(const Vector<t_complex> &y,
              const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> &measure_op,
              const sopt::LinearTransform<Vector<t_complex>> &Psi, const t_real epsilon,
              const std::string &prefix, const t_real oversample_ratio, const t_real channels,
              const t_real imsizex, const t_real measure_op_norm) {
  const Vector<t_complex> dmap = (measure_op->adjoint() * y) / channels * measure_op_norm *
                                 measure_op_norm * oversample_ratio * imsizex;  // in jy/beam
  pfitsio::write2d(dmap.real(), prefix + "_rm_dmap_real.fits");
  pfitsio::write2d(dmap.imag(), prefix + "_rm_dmap_imag.fits");

  auto const padmm =
      sopt::algorithm::ImagingProximalADMM<t_complex>(y)
          .itermax(10000)
          .gamma((Psi.adjoint() * (measure_op->adjoint() * y).eval()).cwiseAbs().maxCoeff() * 1e-3)
          .relative_variation(1e-6)
          .l2ball_proximal_epsilon(epsilon)
          .tight_frame(false)
          .l1_proximal_tolerance(1e-3)
          .l1_proximal_nu(1.)
          .l1_proximal_itermax(50)
          .l1_proximal_positivity_constraint(false)
          .l1_proximal_real_constraint(false)
          .residual_convergence(epsilon)
          .lagrange_update_scale(0.9)
          .nu(1e0)
          .Psi(Psi)
          .Phi(*measure_op);

  auto const diagnostic = padmm();
  Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), 1, imsizex);
  pfitsio::write2d(image.real(), prefix + "_sol_real.fits");
  pfitsio::write2d(image.imag(), prefix + "_sol_imag.fits");
  Vector<t_complex> residuals = measure_op->adjoint() * (y - ((*measure_op) * diagnostic.x)) /
                                channels * measure_op_norm * measure_op_norm * oversample_ratio *
                                imsizex;
  Image<t_complex> residual_image = Image<t_complex>::Map(residuals.data(), 1, imsizex);
  pfitsio::write2d(residual_image.real(), prefix + "_res_real.fits");
  pfitsio::write2d(residual_image.imag(), prefix + "_res_imag.fits");
}
}  // namespace

int main(int nargs, char const **args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");
  sopt::logging::initialize();
  sopt::logging::set_level("debug");

  const t_int channels = 1100;
  const t_real start_c = 700e6;
  const t_real end_c = 1800e6;
  const t_real snr = 30;
  auto const oversample_ratio = 2;
  auto const power_iters = 100;
  auto const power_tol = 1e-4;
  auto const Ju = 4;
  auto const imsizex = 8192;

  auto const kernel = "kb";
  const t_real dnu = (end_c - start_c) / channels;  // Hz
  const t_int Jl_max = 100;
  const Vector<t_real> freq = Vector<t_real>::LinSpaced(channels, start_c, end_c);
  const Vector<t_real> lambda2 = ((constant::c / (freq.array() - dnu)).square() +
                                  (constant::c / (freq.array() + dnu)).square()) /
                                 2;
  const Vector<t_real> widths = ((constant::c / (freq.array() - dnu)).square() -
                                 (constant::c / (freq.array() + dnu)).square());

  const t_real cell = constant::pi / (2 * lambda2.maxCoeff());  // rad/m^2
  const auto measure_op_stuff = sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
          lambda2, widths, Vector<t_complex>::Ones(lambda2.size()), imsizex, cell, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, Jl_max, 1e-6, 1e-6),
      power_iters, power_tol, Vector<t_complex>::Random(imsizex).eval());
  const auto measure_op = std::get<2>(measure_op_stuff);
  const t_real measure_op_norm = std::get<0>(measure_op_stuff);
  const t_real scale = std::sqrt(imsizex * oversample_ratio);

  const auto measure_op_no_correction_stuff =
      sopt::algorithm::normalise_operator<Vector<t_complex>>(
          measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
              lambda2, Vector<t_real>::Zero(lambda2.size()),
              Vector<t_complex>::Ones(lambda2.size()), imsizex, cell, oversample_ratio,
              kernels::kernel_from_string.at(kernel), Ju, Jl_max, 1e-6, 1e-6),
          power_iters, power_tol, Vector<t_complex>::Random(imsizex).eval());
  const auto measure_op_no_correction = std::get<2>(measure_op_no_correction_stuff);
  const t_real measure_op_norm_no_correction = std::get<0>(measure_op_no_correction_stuff);

  Vector<t_complex> input = Vector<t_complex>::Zero(imsizex);
  for (t_int i = 0; i < 15; i++)
    input((i + 1) * 512 - 1) =
        std::exp(-2. * t_complex(0., 1.) * constant::pi * static_cast<t_real>(i) / 8.);

  Vector<t_complex> y = (*measure_op * input);
  t_real const sigma = utilities::SNR_to_standard_deviation(y, snr);

  // adding noise to visibilities
  y = utilities::add_noise(y, 0., sigma);
  auto const epsilon = utilities::calculate_l2_radius(y.size(), sigma);

  sopt::wavelets::SARA const sara{std::make_tuple("Dirac", 3u)};
  //, std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
  //     std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
  //     std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u),
  //  std::make_tuple("DB8", 3u)};

  auto const Psi = sopt::linear_transform<t_complex>(sara, imsizex, 1);

  run_admm(y, measure_op, Psi, epsilon, "corrected", oversample_ratio, channels, imsizex,
           measure_op_norm);
  run_admm(y, measure_op_no_correction, Psi, epsilon, "not_corrected", oversample_ratio, channels,
           imsizex, measure_op_norm_no_correction);
}
