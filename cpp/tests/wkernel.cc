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
TEST_CASE("test transform kernels") {
  SECTION("horizon bound check for 2d transform") {
    REQUIRE(projection_kernels::fourier_wproj_kernel(0, 0, 0, 0, 0, 1, 1).real() == Approx(1));
    REQUIRE(std::abs(projection_kernels::fourier_wproj_kernel(2, 2, 0, 0, 0, 1, 1)) == Approx(0));
    REQUIRE(std::abs(projection_kernels::fourier_wproj_kernel(0.4, 0, 0, 0, 0, 0.3, 0.3)) ==
            Approx(0));
  }
}
TEST_CASE("calculating zero") {
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
      0, 0, 0, du, du, oversample_ratio, normu, normu, 1e9, 0, 1e-12, integration::method::h, e)));
  const t_real norm1d = std::abs(projection_kernels::exact_w_projection_integration_1d(
      0, 0, 0, du, oversample_ratio, normu, 1e9, 0, 1e-12, integration::method::h, e));
  auto const ftkernelu = [=](const t_real l) -> t_real {
    return kernels::ft_kaiser_bessel(l, J) / norm2d;
  };
  auto const ftkernelv = [=](const t_real l) -> t_real {
    return kernels::ft_kaiser_bessel(l, J) / norm1d;
  };
  t_uint total = 0;
  t_uint rtotal = 0;
  t_int const Ju = J;
  t_real const upsample = 2;
  const t_int Ju_max = std::floor(Ju * upsample * 0.5);
  SECTION("+u") {
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      const t_real u = j / upsample;
      CAPTURE(u);
      const t_complex kernel2d = projection_kernels::exact_w_projection_integration(
          u, 0, 0, du, du, oversample_ratio, ftkernelu, ftkernelu, max_evaluations, absolute_error,
          absolute_error, integration::method::h, evaluations);
      const t_complex kernel1d = projection_kernels::exact_w_projection_integration_1d(
          u, 0, 0, du, oversample_ratio, ftkernelv, max_evaluations, absolute_error, absolute_error,
          integration::method::h, evaluations);
      CHECK(kernel2d.real() ==
            Approx(kernels::kaiser_bessel(0, J) * kernels::kaiser_bessel(u, J)).margin(1e-4));
      CHECK(kernel2d.imag() == Approx(0.).margin(1e-5));
      CHECK(kernel1d.imag() == Approx(0.).margin(1e-5));
    }
  }
  SECTION("-u") {
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      const t_real u = j / upsample;
      CAPTURE(u);
      const t_complex kernel2d = projection_kernels::exact_w_projection_integration(
          -u, 0, 0, du, du, oversample_ratio, ftkernelu, ftkernelu, max_evaluations, absolute_error,
          absolute_error, integration::method::h, evaluations);
      CHECK(kernel2d.real() ==
            Approx(kernels::kaiser_bessel(0, J) * kernels::kaiser_bessel(-u, J)).margin(1e-4));
      CHECK(kernel2d.imag() == Approx(0.).margin(1e-5));
    }
  }
  SECTION("+/-w relation") {
    const t_real w = 100.;
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      const t_real u = j / upsample;
      CAPTURE(u);
      const t_complex kernel2d = projection_kernels::exact_w_projection_integration(
          u, 0, w, du, du, oversample_ratio, ftkernelu, ftkernelu, max_evaluations, absolute_error,
          absolute_error, integration::method::h, evaluations);
      const t_complex kernel1d = projection_kernels::exact_w_projection_integration_1d(
          u, 0, w, du, oversample_ratio, ftkernelv, max_evaluations, absolute_error, absolute_error,
          integration::method::h, evaluations);
      const t_complex kernel2d_conj = projection_kernels::exact_w_projection_integration(
          u, 0, -w, du, du, oversample_ratio, ftkernelu, ftkernelu, max_evaluations, absolute_error,
          absolute_error, integration::method::h, evaluations);
      const t_complex kernel1d_conj = projection_kernels::exact_w_projection_integration_1d(
          u, 0, -w, du, oversample_ratio, ftkernelv, max_evaluations, absolute_error,
          absolute_error, integration::method::h, evaluations);
      CHECK(kernel2d.real() == Approx(kernel2d_conj.real()).margin(1e-4));
      CHECK(kernel2d.imag() == Approx(-kernel2d_conj.imag()).margin(1e-4));
      CHECK(kernel1d.real() == Approx(kernel1d_conj.real()).margin(1e-4));
      CHECK(kernel1d.imag() == Approx(-kernel1d_conj.imag()).margin(1e-4));
    }
  }
  SECTION("window function") {
    for (int j = 0; j < Ju_max; j++) {
      t_uint evaluations = 0;
      t_uint revaluations = 0;
      t_uint e;
      const t_real u = j / upsample;
      CAPTURE(u);
      const t_complex kernel2d = projection_kernels::exact_w_projection_integration(
          u, 0, 0, du, du, oversample_ratio, [](t_real) { return 1.; }, [](t_real) { return 1.; },
          max_evaluations, 1e-5, 1e-5, integration::method::h, evaluations);
      const t_complex kernel1d = projection_kernels::exact_w_projection_integration_1d(
          u + 1e-4, 0, 0, du, oversample_ratio, [](t_real) { return 1.; }, max_evaluations, 1e-5,
          1e-5, integration::method::h, evaluations);
      // analytical results from theory
      const t_real expected2d = boost::math::sinc_pi(2 * constant::pi * u * 2. / oversample_ratio);
      const t_real expected1d =
          boost::math::cyl_bessel_j(1, 2 * constant::pi * (u + 1e-4) * 2. / oversample_ratio) /
          ((u + 1e-4) * 2. / oversample_ratio);
      CAPTURE(du);
      CAPTURE(kernel2d / expected2d);
      CAPTURE(kernel1d / expected1d);
      CHECK(kernel2d.real() == Approx(expected2d).margin(1e-6));
      CHECK(kernel2d.imag() == Approx(0).margin(1e-6));
      CHECK(kernel1d.real() == Approx(expected1d).margin(1e-6));
      CHECK(kernel1d.imag() == Approx(0).margin(1e-6));
    }
  }
}
