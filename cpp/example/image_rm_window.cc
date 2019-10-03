#include "purify/config.h"
#include "purify/types.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/rm_operators.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>

#include <boost/math/special_functions/sinc.hpp>

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(width, 1, 10, t_real)
  ARGS_MACRO(imsize, 2, 256, t_uint)
  ARGS_MACRO(cell, 3, 2400, t_real)
  ARGS_MACRO(Jl_max, 4, 0, t_uint)
#undef ARGS_MACRO
  purify::logging::initialize();
  purify::logging::set_level("debug");
  // Gridding example
  auto const oversample_ratio = 2;
  auto const power_iters = 100;
  auto const power_tol = 1e-5;
  auto const Ju = 4;
  t_int const imsizex = imsize;
  const std::string suffix = "_" + std::to_string(static_cast<int>(width)) + "_" +
                             std::to_string(static_cast<int>(imsize)) + "_" +
                             std::to_string(static_cast<int>(cell));

  auto const kernel = "kb";

  t_uint const number_of_pixels = imsizex;
  t_uint const number_of_vis = std::floor(number_of_pixels * 2);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;

  auto header =
      pfitsio::header_params("", " ", 1, 0, 0, stokes::I, cell, cell, 0, 1, 0, 0, 0, 0, 0);
  const auto measure_opw = measurementoperator::init_degrid_operator_1d<Vector<t_complex>>(
      Vector<t_real>::Zero(1), Vector<t_real>::Constant(1, width), Vector<t_complex>::Ones(1),
      imsizex, cell, oversample_ratio, kernels::kernel_from_string.at(kernel), Ju, Jl_max, 1e-6,
      1e-6);
  Vector<t_complex> const measurements =
      (measure_opw->adjoint() * Vector<t_complex>::Constant(1, 1)).conjugate();
  t_uint max_pos = 0;
  measurements.array().real().maxCoeff(&max_pos);
  Image<t_complex> out = measurements * std::sqrt(imsizex * oversample_ratio);
  header.fits_name = "rm_window_real" + suffix + ".fits";
  pfitsio::write2d(out.real(), header);
  header.fits_name = "rm_window_imag" + suffix + ".fits";
  pfitsio::write2d(out.imag(), header);
  Vector<t_complex> true_window = Vector<t_complex>::Zero(imsize);
  for (t_int i = 0; i < imsize; i++)
    true_window(i) = boost::math::sinc_pi(width * std::abs(i - (imsize) / 2.) * cell);
  header.fits_name = "true_window_real" + suffix + ".fits";
  pfitsio::write2d(true_window.real(), header);
}
