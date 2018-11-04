#include "purify/config.h"
#include "catch.hpp"
#include "purify/logging.h"

#include "purify/types.h"

#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/wide_field_utilities.h"
#include "purify/wkernel_integration.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("zero w") {
  const t_real cell = 30;
  const t_int imsize = 1024;
  const t_real absolute_error = 1e-9;
  t_uint const J = 4;
  const t_real oversample_ratio = 2;
  const t_real du = widefield::pixel_to_lambda(cell, imsize, oversample_ratio);
  auto const normu = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J); };

  const t_uint max_evaluations = 1e9;
  const t_real relative_error = 0;
  t_uint e;
  const t_real norm2d = std::sqrt(std::abs(projection_kernels::exact_w_projection_integration(
      0, 0, 0, du, du, normu, normu, 1e9, 0, 1e-12, integration::method::h, false, e)));
  const t_real norm1d = std::abs(projection_kernels::exact_w_projection_integration_1d(
      0, 0, 0, du, normu, 1e9, 0, 1e-12, integration::method::h, e));
  auto const ftkernelu = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J)/norm2d; };
  auto const ftkernelv = [=](const t_real l) -> t_real { return kernels::ft_kaiser_bessel(l, J)/norm1d; };
  t_uint total = 0;
  t_uint rtotal = 0;
  t_int const Ju = J;
  t_real const upsample = 10;
  const t_int Ju_max = std::floor(Ju * upsample * 0.5);
  for (int j = 0; j < Ju_max; j++) {
    t_uint evaluations = 0;
    t_uint revaluations = 0;
    t_uint e;
    const t_real u = j / upsample;
    CAPTURE(u);
    const t_complex kernel2d =
        projection_kernels::exact_w_projection_integration(
            u, 0, 0, du, du, ftkernelu, ftkernelu, max_evaluations, absolute_error, absolute_error,
            integration::method::h, false, evaluations);
    const t_complex kernel1d = projection_kernels::exact_w_projection_integration_1d(
                                   u, 0, 0, du, ftkernelv, max_evaluations,
                                   absolute_error, absolute_error, integration::method::h, evaluations);
    CHECK(kernel2d.real() ==
          Approx(kernels::kaiser_bessel(0, J) * kernels::kaiser_bessel(u, J)).margin(1e-4));
    CHECK(kernel2d.imag() == Approx(0.).margin(1e-5));
    CHECK(kernel1d.imag() == Approx(0.).margin(1e-5));
  }
}
