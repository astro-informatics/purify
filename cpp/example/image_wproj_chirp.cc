#include "purify/config.h"
#include "purify/types.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/wproj_operators.h"
#include <sopt/power_method.h>

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(w, 1, 10, t_real)
  ARGS_MACRO(imsize, 2, 256, t_uint)
  ARGS_MACRO(cell, 3, 2400, t_real)
  ARGS_MACRO(Jw_max, 4, 0, t_uint)
#undef ARGS_MACRO
  purify::logging::initialize();
  purify::logging::set_level("debug");
  // Gridding example
  auto const oversample_ratio = 2;
  auto const power_iters = 0;
  auto const power_tol = 1e-5;
  auto const Ju = 4;
  auto const Jv = 4;
  auto const imsizex = imsize;
  auto const imsizey = imsize;
  const t_real wval = w;
  const t_int Jw = (Jw_max > 0) ? Jw_max : widefield::w_support(w, widefield::pixel_to_lambda(cell, imsize, oversample_ratio), Ju, 1000);
  const std::string suffix = "_" + std::to_string(static_cast<int>(wval)) + "_" +
                             std::to_string(static_cast<int>(imsize)) + "_" +
                             std::to_string(static_cast<int>(cell)) + "_" +
                             std::to_string(static_cast<int>(Jw_max));

  auto const kernel = "kb";

  t_uint const number_of_pixels = imsizex * imsizey;
  t_uint const number_of_vis = std::floor(number_of_pixels * 2);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;
  auto uv_vis = utilities::random_sample_density(1, 0, sigma_m);
  uv_vis.u *= 0.;
  uv_vis.v *= 0.;
  uv_vis.w = Vector<t_real>::Constant(1, wval);
  uv_vis.units = utilities::vis_units::radians;
  Image<t_complex> chirp = details::init_correction2d(
      2, imsizey, imsizex, [](t_real) { return 1.; }, [](t_real) { return 1.; }, wval, cell, cell);

  pfitsio::write2d(chirp.real() / chirp.real().maxCoeff(),
                   "chirp_real_" + std::to_string(static_cast<int>(wval)) + "_" +
                       std::to_string(static_cast<int>(imsize)) + "_" +
                       std::to_string(static_cast<int>(cell)) + ".fits");
  pfitsio::write2d(chirp.imag() / chirp.real().maxCoeff(),
                   "chirp_imag_" + std::to_string(static_cast<int>(wval)) + "_" +
                       std::to_string(static_cast<int>(imsize)) + "_" +
                       std::to_string(static_cast<int>(cell)) + ".fits");
  const auto measure_opw = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_vis, imsizey, imsizex, cell, cell, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, Jw, false, 1e-6, 1e-6),
      power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey)));
  Vector<t_complex> const measurements =
      (measure_opw->adjoint() * Vector<t_complex>::Constant(1, 1)).conjugate();
  t_uint max_pos = 0;
  measurements.array().real().maxCoeff(&max_pos);
  Image<t_complex> out =
      Image<t_complex>::Map(measurements.data(), imsizey, imsizex) / measurements(max_pos);
  pfitsio::write2d(out.real(), "wproj_real" + suffix + ".fits");
  pfitsio::write2d(out.imag(), "wproj_imag" + suffix + ".fits");
  pfitsio::write2d((chirp - out).real(), "diff_real" + suffix + ".fits");
  pfitsio::write2d((chirp - out).imag(), "diff_imag" + suffix + ".fits");
}
