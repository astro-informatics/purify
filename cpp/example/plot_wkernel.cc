
#include "purify/config.h"

#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"

#include "purify/types.h"
#include "purify/logging.h"

#include "purify/kernels.h"

#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/wide_field_utilities.h"
#include "purify/wkernel_integration.h"

using namespace purify;
using namespace purify::notinstalled;

using namespace purify;

int main(int nargs, char const **args) {
  auto const output_name = (nargs > 2) ? static_cast<std::string>(args[1]) : "kernel";
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(w, 2, 10, t_real)
  ARGS_MACRO(absolute_error, 3, 1e-2, t_real)
  ARGS_MACRO(imsize, 4, 1024, t_uint)
  ARGS_MACRO(cell, 5, 20, t_real)
  ARGS_MACRO(radial, 6, false, bool)

#undef ARGS_MACRO
  purify::logging::set_level("debug");
  t_uint const J = 4;
  t_int const Jw = 30;
  const t_real oversample_ratio = 2;
  PURIFY_LOW_LOG("image size: {}", imsize);
  PURIFY_LOW_LOG("cell size: {}", cell);
  const t_real du = widefield::pixel_to_lambda(cell, imsize, oversample_ratio);
  PURIFY_LOW_LOG("1/du: {}", 1. / du);
  auto const normu = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J); };

  const t_uint max_evaluations = 1e8;
  const t_real relative_error = 0;
  t_uint e;
  const t_real norm =
      (not radial)
          ? std::abs(projection_kernels::exact_w_projection_integration(
                0, 0, 0, du, du, oversample_ratio, normu, normu, 1e9, 0, 1e-8,
                integration::method::h, e))
          : std::abs(projection_kernels::exact_w_projection_integration_1d(
                0, 0, 0, du, oversample_ratio, normu, 1e9, 0, 1e-7, integration::method::h, e));
  auto const ftkernelu = [=](const t_real l) -> t_real {
    return kernels::ft_kaiser_bessel(l, J) / norm;
  };
  auto const ftkernelv = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J); };
  t_uint total = 0;
  t_uint rtotal = 0;
  t_int const Ju = 20;
  t_real const upsample = 5;
  t_int const Ju_max = std::floor(Ju * upsample);
  const t_int Jw_max =
      100;  // std::min<int>(std::max<int>(std::floor(std::abs(w * 1./du)), J), Jw);
  Vector<t_complex> kernel = Vector<t_complex>::Zero(Ju_max * Jw_max);
  Vector<t_real> evals = Vector<t_real>::Zero(Ju_max * Jw_max);
  Vector<t_real> support = Vector<t_real>::Zero(Jw_max);
  PURIFY_LOW_LOG("w: {}", w);
  PURIFY_LOW_LOG("Kernel size: {} x {}", Ju_max, Jw_max);
  PURIFY_LOW_LOG("FoV (deg): {}", imsize * cell / 60 / 60);
  PURIFY_LOW_LOG("du: {}", du);
#pragma omp parallel for collapse(2)
  for (int i = 0; i < Jw_max; i++) {
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      kernel(j * Jw_max + i) =
          (not radial)
              ? projection_kernels::exact_w_projection_integration(
                    j / upsample, 0, w * i / Jw_max, du, du, oversample_ratio, ftkernelu, ftkernelv,
                    max_evaluations, absolute_error, 0, integration::method::h, evaluations)
              : projection_kernels::exact_w_projection_integration_1d(
                    j / upsample, 0, w * i / Jw_max, du, oversample_ratio, ftkernelu,
                    max_evaluations, absolute_error, 0, integration::method::p, evaluations);
      evals(j * Jw_max + i) = evaluations;
#pragma omp critical(print)
      {
        PURIFY_LOW_LOG("u : {}, w : {}", j / upsample, w * i / Jw_max);
        PURIFY_LOW_LOG("Kernel value: {}", kernel(j * Jw_max + i));
        PURIFY_LOW_LOG("Evaluations: {}", evaluations);
      }
      if (kernel(j * Jw_max + i) != kernel(j * Jw_max + i))
        throw std::runtime_error("Kernel value is a nan.");
      total += evaluations;
    }
  }
  PURIFY_LOW_LOG("Total Evaluations: {}", total);
  PURIFY_LOW_LOG("FoV (deg): {}", imsize * cell / 60 / 60);
  PURIFY_LOW_LOG("du: {}", du);
  pfitsio::write2d(kernel.real(), Jw_max, Ju_max, output_name + "_real.fits");
  pfitsio::write2d(kernel.imag(), Jw_max, Ju_max, output_name + "_imag.fits");
  pfitsio::write2d(kernel.cwiseAbs(), Jw_max, Ju_max, output_name + ".fits");
  pfitsio::write2d(evals, Jw_max, Ju_max, output_name + "_evals.fits");
}
