
#include "purify/config.h"

#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"

#include "purify/types.h"
#include "purify/logging.h"

#include "purify/kernels.h"

#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/rm_kernel_integration.h"

using namespace purify;
using namespace purify::notinstalled;

using namespace purify;

int main(int nargs, char const **args) {
  auto const output_name = (nargs > 2) ? static_cast<std::string>(args[1]) : "kernel";
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(dl2, 2, 10, t_real)
  ARGS_MACRO(absolute_error, 3, 1e-2, t_real)
  ARGS_MACRO(imsize, 4, 1024, t_uint)
  ARGS_MACRO(cell, 5, 20, t_real)

#undef ARGS_MACRO
  purify::logging::initialize();
  purify::logging::set_level("debug");
  t_uint const J = 4;
  t_int const Jl = 30;
  const t_real oversample_ratio = 2;
  PURIFY_LOW_LOG("image size: {}", imsize);
  PURIFY_LOW_LOG("cell size: {}", cell);
  const t_real du = constant::pi / (oversample_ratio * cell * imsize);
  PURIFY_LOW_LOG("1/du: {}", 1. / du);
  auto const normu = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J); };

  const t_uint max_evaluations = 1e8;
  const t_real relative_error = 0;
  t_uint e;
  const t_real norm = std::abs(projection_kernels::exact_rm_projection_integration(
      0, 0, du, oversample_ratio, normu, 1e9, 0, 1e-8, integration::method::p, e));
  auto const ftkernelu = [=](const t_real l) -> t_real {
    return kernels::ft_kaiser_bessel(l, J) / norm;
  };
  t_uint total = 0;
  t_uint rtotal = 0;
  t_int const Ju = 20;
  t_real const upsample = 5;
  t_int const Ju_max = std::floor(Ju * upsample);
  const t_int Jl_max = 100;
  Vector<t_complex> kernel = Vector<t_complex>::Zero(Ju_max * Jl_max);
  Vector<t_real> evals = Vector<t_real>::Zero(Ju_max * Jl_max);
  Vector<t_real> support = Vector<t_real>::Zero(Jl_max);
  PURIFY_LOW_LOG("dlambda^2: {}", dl2);
  PURIFY_LOW_LOG("Kernel size: {} x {}", Ju_max, Jl_max);
  PURIFY_LOW_LOG("FoV (rad/m^2): {}", imsize * cell);
  PURIFY_LOW_LOG("du: {}", du);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < Jl_max; i++) {
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      if (j / upsample < (dl2 * i / Jl_max / du + J) / 2) {
        kernel(j * Jl_max + i) = projection_kernels::exact_rm_projection_integration(
            j / upsample, dl2 * i / Jl_max, du, oversample_ratio, ftkernelu, max_evaluations,
            absolute_error, 0, integration::method::p, evaluations);
        evals(j * Jl_max + i) = evaluations;
#pragma omp critical(print)
        {
          PURIFY_LOW_LOG("u : {}, dlambda^2 : {}", j / upsample, dl2 * i / Jl_max);
          PURIFY_LOW_LOG("Kernel value: {}", kernel(j * Jl_max + i));
          PURIFY_LOW_LOG("Evaluations: {}", evaluations);
        }
        if (kernel(j * Jl_max + i) != kernel(j * Jl_max + i))
          throw std::runtime_error("Kernel value is a nan.");
        total += evaluations;
      }
    }
  }
  PURIFY_LOW_LOG("Total Evaluations: {}", total);
  PURIFY_LOW_LOG("FoV (rad/m^2): {}", imsize * cell);
  PURIFY_LOW_LOG("du: {}", du);
  pfitsio::write2d(kernel.real(), Jl_max, Ju_max, output_name + "_real.fits");
  pfitsio::write2d(kernel.imag(), Jl_max, Ju_max, output_name + "_imag.fits");
  pfitsio::write2d(kernel.cwiseAbs(), Jl_max, Ju_max, output_name + ".fits");
  pfitsio::write2d(evals, Jl_max, Ju_max, output_name + "_evals.fits");
}
