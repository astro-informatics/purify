#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>

using namespace purify;

int main(int nargs, char const **args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");
  // Gridding example
  auto const oversample_ratio = 2;
  auto const power_iters = 0;
  auto const power_tol = 1e-4;
  auto const Ju = 4;
  auto const Jv = 4;
  auto const imsizex = 256;
  auto const imsizey = 256;

  auto const kernel = "kb";

  t_uint const number_of_pixels = imsizex * imsizey;
  t_uint const number_of_vis = 4e6;  // std::floor(number_of_pixels * 2);
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;
  auto uv_vis = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_vis.units = utilities::vis_units::radians;
  const Vector<t_complex> image = Vector<t_complex>::Random(number_of_pixels);
  const auto measure_op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, Jv),
      power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey)));
  for (t_uint i = 0; i < 100; i++) {
    PURIFY_LOW_LOG("Iteration: {}", i);
    Vector<t_complex> const measurements = *measure_op * image;
  }
}
