#include "purify/config.h"
#include "purify/types.h"
#include <chrono>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/wide_field_utilities.h"
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
#undef ARGS_MACRO
  purify::logging::initialize();
  purify::logging::set_level("debug");
  // Gridding example
  auto const oversample_ratio = 2;
  auto const power_iters = 1000;
  auto const power_tol = 1e-6;
  auto const Ju = 4;
  auto const Jv = 4;
  auto const imsizex = imsize;
  auto const imsizey = imsize;
  const t_real wval = w;
  const t_int total = 10;
  auto const kernel = "kb";
  std::vector<t_real> M(10);
  std::vector<t_real> ctor_mop_wr(10);
  std::vector<t_real> ctor_mop_w2d(10);
  std::vector<t_real> ctor_mop(10);
  std::vector<t_real> diff_wr_w2d(10);
  std::vector<t_real> diff_mop_w2d(10);
  for (t_int i = 1; i < M.size() + 1; i++) {
    t_uint const number_of_vis = 100 * i;
    t_uint trial = 0;
    M[i - 1] = number_of_vis;
    // Generating random uv(w) coverage
    while (trial < total) {
      t_real const sigma_m = constant::pi / 3;
      auto uv_vis = utilities::random_sample_density(number_of_vis, 0, sigma_m, wval);
      uv_vis.units = utilities::vis_units::radians;

      auto start = std::chrono::high_resolution_clock::now();
      auto mop_radial = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_vis, imsizey, imsizex, cell, cell, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, 40, false, 1e-6, 1e-6,
          dde_type::wkernel_radial);
      auto end = std::chrono::high_resolution_clock::now();
      ctor_mop_wr[i - 1] +=
          std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() / total;
      // normalise operator
      mop_radial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          mop_radial, power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey).eval()));

      start = std::chrono::high_resolution_clock::now();
      auto mop_2d = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_vis, imsizey, imsizex, cell, cell, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, 40, false, 1e-6, 1e-6, dde_type::wkernel_2d);
      end = std::chrono::high_resolution_clock::now();
      ctor_mop_w2d[i - 1] +=
          std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() / total;
      // normalise operator
      mop_2d = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          mop_2d, power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey).eval()));

      start = std::chrono::high_resolution_clock::now();
      auto mop = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_vis, imsizey, imsizex, cell, cell, oversample_ratio,
          kernels::kernel_from_string.at(kernel), Ju, Ju, false);
      end = std::chrono::high_resolution_clock::now();
      ctor_mop[i - 1] +=
          std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() / total;
      // normalise operator
      mop = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          mop, power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey).eval()));
      diff_wr_w2d[i - 1] +=
          std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
              {[=](Vector<t_complex> &out, const Vector<t_complex> &x) {
                 out = ((*mop_radial) * x) - ((*mop_2d) * x);
               },
               mop_radial->sizes(),
               [=](Vector<t_complex> &out, const Vector<t_complex> &x) {
                 out = (mop_radial->adjoint() * x) - (mop_2d->adjoint() * x);
               },
               mop_radial->adjoint().sizes()},
              power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey).eval())) /
          total;
      diff_mop_w2d[i - 1] +=
          std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
              {[=](Vector<t_complex> &out, const Vector<t_complex> &x) {
                 out = ((*mop_2d) * x) - ((*mop) * x);
               },
               mop_2d->sizes(),
               [=](Vector<t_complex> &out, const Vector<t_complex> &x) {
                 out = (mop_2d->adjoint() * x) - (mop->adjoint() * x);
               },
               mop_2d->adjoint().sizes()},
              power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey).eval())) /
          total;
      trial++;
    }
  }
  for (t_uint i = 0; i < M.size(); i++)
    std::cout << M.at(i) << " " << ctor_mop_wr.at(i) << " " << ctor_mop_w2d.at(i) << " "
              << ctor_mop.at(i) << " " << diff_wr_w2d.at(i) << " " << diff_mop_w2d.at(i)
              << std::endl;
}
