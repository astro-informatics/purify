#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/logging.h"

#include "purify/kernels.h"
#include "purify/spherical_resample.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("test basic l m n calculation") {
  SECTION("north pole") {
    const t_real theta_0 = 0;
    const t_real phi_0 = 0;
    SECTION("l = 0, m = 0, n = 1") {
      const t_real l = spherical_resample::calculate_l(theta_0, phi_0);
      const t_real m = spherical_resample::calculate_m(theta_0, phi_0);
      const t_real n = spherical_resample::calculate_n(phi_0);
      CHECK(l == Approx(0.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(1.));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta = 0;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta, phi);
      const t_real m = spherical_resample::calculate_m(theta, phi);
      const t_real n = spherical_resample::calculate_n(phi);
      CHECK(l == Approx(1.));
      CHECK(m == Approx(0.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta = constant::pi / 2.;
      const t_real phi = constant::pi / 2.;
      const t_real l = spherical_resample::calculate_l(theta, phi);
      const t_real m = spherical_resample::calculate_m(theta, phi);
      const t_real n = spherical_resample::calculate_n(phi);
      CHECK(l == Approx(0.).margin(1e-12));
      CHECK(m == Approx(1.));
      CHECK(n == Approx(0.).margin(1e-12));
    }
  }
  SECTION("rotations") {
    SECTION("l = 0, m = 0, n = 1") {
      const t_real theta_0 = -constant::pi / 2.;
      const t_real phi_0 = constant::pi / 3.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., 0., phi_0, theta_0)));
    }
    SECTION("l = 1, m = 0, n = 0") {
      const t_real theta_0 = 0;
      const t_real phi_0 = constant::pi / 2.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., theta_0, phi_0, 0.)));
    }
    SECTION("l = 0, m = 1, n = 0") {
      const t_real theta_0 = constant::pi / 2.;
      const t_real phi_0 = constant::pi / 2.;
      CHECK(spherical_resample::calculate_l(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_l(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_m(theta_0, phi_0) ==
            Approx(spherical_resample::calculate_m(0., 0., theta_0, phi_0, 0.)));
      CHECK(spherical_resample::calculate_n(phi_0) ==
            Approx(spherical_resample::calculate_n(0., 0., theta_0, phi_0, 0.)));
    }
  }
}

TEST_CASE("select resamples") {
  const t_int imsizex = 1024;
  const t_int imsizey = imsizex;
  const t_real dl = 2. / imsizex;
  const t_real dm = 2. / imsizey;
  const t_int N = 1000;
  const Vector<t_real> l = Vector<t_real>::Random(N);
  const Vector<t_real> m = Vector<t_real>::Random(N);
  const Vector<t_real> n = Vector<t_real>::Random(N);
  const std::vector<t_int> indicies =
      spherical_resample::generate_indicies(l, m, n, imsizey * dm, imsizex * dl);
  CAPTURE(l.head(20).transpose());
  CAPTURE(m.head(20).transpose());
  CAPTURE(n.head(20).transpose());
  REQUIRE(indicies.size() > 0);
  for (t_int k = 0; k < indicies.size(); k++) {
    CHECK(std::abs(l(indicies.at(k))) < dl * imsizex * 0.5);
    CHECK(std::abs(m(indicies.at(k))) < dm * imsizex * 0.5);
    CHECK(n(indicies.at(k)) > 0);
  }
}

TEST_CASE("Test FT Scaling and Padding") {
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int imsizex = 256;
  const t_int imsizey = 256;
  const t_real oversample_ratio = 2.;

  const auto kernelstuff = purify::create_kernels(kernels::kernel_from_string.at("kb"), Jl, Jm,
                                                  imsizey, imsizex, oversample_ratio);

  const std::function<t_real(t_real)>& kernell = std::get<0>(kernelstuff);
  const std::function<t_real(t_real)>& kernelm = std::get<1>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernell = std::get<2>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernelm = std::get<3>(kernelstuff);

  const Image<t_complex> S_u = purify::details::init_correction2d(
      oversample_ratio, imsizey, imsizex, ftkernell, ftkernelm, 0., 1., 1.);

  auto FT_Z_op =
      spherical_resample::init_FT_zero_padding_2d<Vector<t_complex>>(S_u, oversample_ratio);
  SECTION("Forward") {
    const Vector<t_complex> input = Vector<t_complex>::Ones(std::floor(imsizex * oversample_ratio) *
                                                            std::floor(imsizex * oversample_ratio));
    Vector<t_complex> output;
    std::get<0>(FT_Z_op)(output, input);
    CHECK(output.size() == imsizex * imsizey);
    const Image<t_complex>& output_grid = Image<t_complex>::Map(output.data(), imsizey, imsizex);
    CHECK(output_grid.topLeftCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.bottomRightCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.topRightCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.bottomLeftCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.bottomRightCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.topLeftCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.bottomLeftCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.topRightCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
  }
  SECTION("Backward") {
    const Vector<t_complex> input =
        Vector<t_complex>::Ones(std::floor(imsizex) * std::floor(imsizex));
    Vector<t_complex> output;
    std::get<1>(FT_Z_op)(output, input);
    CHECK(output.size() ==
          std::floor(imsizex * oversample_ratio) * std::floor(imsizex * oversample_ratio));
    const Image<t_complex>& output_grid =
        Image<t_complex>::Map(output.data(), std::floor(imsizey * oversample_ratio),
                              std::floor(imsizex * oversample_ratio));
    CHECK(output_grid.topLeftCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.bottomRightCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.topRightCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.bottomLeftCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.bottomRightCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.topLeftCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
    CHECK(output_grid.bottomLeftCorner(imsizey * 0.5, imsizex * 0.5)
              .real()
              .isApprox(S_u.topRightCorner(imsizey * 0.5, imsizex * 0.5).real(), 1e-6));
  }
}

TEST_CASE("Test FFT Correction") {
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_real oversample_ratio = 2.;
  const t_real oversample_ratio_image_domain = 2.;
  const t_int imsizex = 256;
  const t_int imsizey = 256;
  const auto ft_plan = operators::fftw_plan::measure;

  const Vector<t_real> l = Vector<t_real>::Zero(1);
  const Vector<t_real> m = Vector<t_real>::Zero(1);
  const auto kernelstuff = purify::create_kernels(kernels::kernel_from_string.at("kb"), Jl, Jm,
                                                  imsizey, imsizex, oversample_ratio_image_domain);

  const std::function<t_real(t_real)>& kernell = std::get<0>(kernelstuff);
  const std::function<t_real(t_real)>& kernelm = std::get<1>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernell = std::get<2>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernelm = std::get<3>(kernelstuff);

  const Sparse<t_complex> interrpolation_matrix = spherical_resample::init_resample_matrix_2d(
      l, m, std::floor(imsizey * oversample_ratio_image_domain),
      std::floor(imsizex * oversample_ratio_image_domain), kernell, kernelm, Jl, Jm,
      [](t_real, t_real) { return 1.; });

  const Vector<t_complex> point_source = Vector<t_complex>::Ones(1);
  const Vector<t_complex> image_on_plane = interrpolation_matrix.adjoint() * point_source;

  SECTION("FFT operation") {
    const Image<t_complex> S_l = purify::details::init_correction2d(
        oversample_ratio, std::floor(imsizey * oversample_ratio_image_domain),
        std::floor(imsizex * oversample_ratio_image_domain), [](t_real x) { return 1.; },
        [](t_real x) { return 1.; }, 0., 0., 0.);

    const auto Z_image_domain_op =
        purify::operators::init_zero_padding_2d<Vector<t_complex>>(S_l, oversample_ratio);
    Vector<t_complex> padded_image;

    std::get<0>(Z_image_domain_op)(padded_image, image_on_plane);
    CHECK(padded_image.size() ==
          std::floor(imsizey * oversample_ratio * oversample_ratio_image_domain) *
              std::floor(imsizex * oversample_ratio * oversample_ratio_image_domain));

    const auto FFT_op = purify::operators::init_FFT_2d<Vector<t_complex>>(
        std::floor(imsizey * oversample_ratio_image_domain),
        std::floor(imsizex * oversample_ratio_image_domain), oversample_ratio, ft_plan);
    Vector<t_complex> ft_grid;
    std::get<0>(FFT_op)(ft_grid, padded_image);
    CHECK(ft_grid.size() ==
          std::floor(imsizey * oversample_ratio * oversample_ratio_image_domain) *
              std::floor(imsizex * oversample_ratio * oversample_ratio_image_domain));
    const Image<t_complex> S_u = purify::details::init_correction2d(
        oversample_ratio_image_domain, std::floor(imsizey * oversample_ratio),
        std::floor(imsizex * oversample_ratio),
        [oversample_ratio, &ftkernell](t_real x) { return ftkernell(x / oversample_ratio); },
        [oversample_ratio, &ftkernelm](t_real x) { return ftkernelm(x / oversample_ratio); }, 0.,
        0., 0.);

    auto const Z_ft_domain_op = spherical_resample::init_FT_zero_padding_2d<Vector<t_complex>>(
        S_u, oversample_ratio_image_domain);

    Vector<t_complex> ft_grid_cropped;
    std::get<0>(Z_ft_domain_op)(ft_grid_cropped, ft_grid);
    CHECK(ft_grid_cropped.size() ==
          std::floor(imsizey * oversample_ratio) * std::floor(imsizex * oversample_ratio));
    const auto ZFZ_op = spherical_resample::base_padding_and_FFT_2d<Vector<t_complex>>(
        [](t_real x) { return 1.; }, [](t_real x) { return 1.; }, ftkernell, ftkernelm, imsizey,
        imsizex, oversample_ratio, oversample_ratio_image_domain, ft_plan);
    Vector<t_complex> output;
    std::get<0>(ZFZ_op)(output, image_on_plane);
    CHECK(output.size() ==
          std::floor(imsizey * oversample_ratio) * std::floor(imsizex * oversample_ratio));
    CHECK(output.isApprox(ft_grid_cropped, 1e-6));
  }
  SECTION("FFT with radial correction") {
    const auto ZFZ_op = spherical_resample::base_padding_and_FFT_2d<Vector<t_complex>>(
        [](t_real x) { return 1.; }, [](t_real x) { return 1.; }, ftkernell, ftkernelm, imsizey,
        imsizex, oversample_ratio, oversample_ratio_image_domain, ft_plan);
    const auto ZFZ_radial_op = spherical_resample::base_padding_and_FFT_2d<Vector<t_complex>>(
        [](t_real x) { return 1.; }, ftkernell, ftkernelm, imsizey, imsizex, oversample_ratio,
        oversample_ratio_image_domain, ft_plan);
    Vector<t_complex> output;
    std::get<0>(ZFZ_op)(output, image_on_plane);
    Vector<t_complex> output_radial;
    std::get<0>(ZFZ_op)(output_radial, image_on_plane);
    CHECK(output.size() == output_radial.size());
    CHECK(output.isApprox(output_radial, 1e-6));
  }
}

TEST_CASE("planar grid degrid") {
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_real oversample_ratio = 2.;
  const t_real oversample_ratio_image_domain = 1.;
  const t_int imsizex = 256;
  const t_int imsizey = 256;
  const auto ft_plan = operators::fftw_plan::measure;
  auto const kernel = kernels::kernel_from_string.at("kb");
  const Vector<t_real> u = Vector<t_real>::Zero(1);
  const Vector<t_real> v = Vector<t_real>::Zero(1);
  const Vector<t_real> w = Vector<t_real>::Zero(1).array();
  const Vector<t_complex> weights = Vector<t_complex>::Ones(1);
  SECTION("no projection") {
    auto const uvkernels =
        purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
    const std::function<t_real(t_real)>& kernelu = std::get<0>(uvkernels);
    const std::function<t_real(t_real)>& kernelv = std::get<1>(uvkernels);
    const std::function<t_real(t_real)>& ftkernelu = std::get<2>(uvkernels);
    const std::function<t_real(t_real)>& ftkernelv = std::get<3>(uvkernels);

    auto const lmkernels =
        purify::create_kernels(kernel, Jl, Jm, imsizey, imsizex, oversample_ratio_image_domain);
    const std::function<t_real(t_real)>& kernell = std::get<0>(lmkernels);
    const std::function<t_real(t_real)>& kernelm = std::get<1>(lmkernels);
    const std::function<t_real(t_real)>& ftkernell = std::get<2>(lmkernels);
    const std::function<t_real(t_real)>& ftkernelm = std::get<3>(lmkernels);

    sopt::OperatorFunction<Vector<t_complex>> directZFZ, indirectZFZ;
    std::tie(directZFZ, indirectZFZ) =
        spherical_resample::base_padding_and_FFT_2d<Vector<t_complex>>(
            ftkernelu, ftkernelv, [](t_real) { return 1.; }, [](t_real) { return 1.; }, imsizey,
            imsizex, oversample_ratio, oversample_ratio_image_domain, ft_plan);

    sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<Vector<t_complex>>(
        u, v, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, Ju, Jv);

    const auto direct = sopt::chained_operators<Vector<t_complex>>(directG, directZFZ);
    const auto indirect = sopt::chained_operators<Vector<t_complex>>(indirectZFZ, indirectG);
    auto const measure_op = operators::base_degrid_operator_2d<Vector<t_complex>>(
        u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, false, 1, 1);
    const auto& forward_expected_op = std::get<0>(measure_op);
    const auto& adjoint_expected_op = std::get<1>(measure_op);
    SECTION("Forward") {
      const Vector<t_complex> input = Vector<t_complex>::Ones(
          imsizex * imsizey * oversample_ratio_image_domain * oversample_ratio_image_domain);
      Vector<t_complex> output;
      direct(output, input);
      Vector<t_complex> output_expected;
      forward_expected_op(output_expected, input);
      CHECK(output.size() == 1);
      CHECK(std::imag(output(0)) == Approx(0.).margin(1e-3));
      CAPTURE(output_expected.head(1));
      CAPTURE(output.head(1));
      CHECK(output.isApprox(output_expected, 1e-6));
    }
    SECTION("adjoint") {
      const Vector<t_complex> input = Vector<t_complex>::Ones(1);
      Vector<t_complex> output;
      indirect(output, input);
      CAPTURE(output.head(5));
      CAPTURE(input);
      Vector<t_complex> output_expected;
      adjoint_expected_op(output_expected, input);
      CAPTURE(output_expected.head(5));
      CHECK(output.isApprox(output_expected, 1e-6));
    }
  }
  SECTION("projection") {
    auto const uvkernels = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    const std::function<t_real(t_real)>& ftkerneluv = std::get<0>(uvkernels);
    const std::function<t_real(t_real)>& kerneluv = std::get<1>(uvkernels);

    auto const lmkernels =
        purify::create_kernels(kernel, Jl, Jm, imsizey, imsizex, oversample_ratio_image_domain);
    const std::function<t_real(t_real)>& kernell = std::get<0>(lmkernels);
    const std::function<t_real(t_real)>& kernelm = std::get<1>(lmkernels);
    const std::function<t_real(t_real)>& ftkernell = std::get<2>(lmkernels);
    const std::function<t_real(t_real)>& ftkernelm = std::get<3>(lmkernels);

    sopt::OperatorFunction<Vector<t_complex>> directZFZ, indirectZFZ;
    std::tie(directZFZ, indirectZFZ) =
        spherical_resample::base_padding_and_FFT_2d<Vector<t_complex>>(
            ftkerneluv, [](t_real) { return 1.; }, [](t_real) { return 1.; }, imsizey, imsizex,
            oversample_ratio, oversample_ratio_image_domain, ft_plan);

    const t_real du = widefield::pixel_to_lambda(1., imsizex, oversample_ratio);
    const t_real dv = widefield::pixel_to_lambda(1., imsizey, oversample_ratio);
    const t_real absolute_error = 1e-8;
    const t_real relative_error = 1e-8;
    const t_int Jw = 100;
    sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<Vector<t_complex>>(
        u, v, w, weights, imsizey, imsizex, oversample_ratio, ftkerneluv, kerneluv, Ju, Jw, du, dv,
        absolute_error, relative_error, dde_type::wkernel_radial);

    const auto direct = sopt::chained_operators<Vector<t_complex>>(directG, directZFZ);
    const auto indirect = sopt::chained_operators<Vector<t_complex>>(indirectZFZ, indirectG);
    auto const measure_op = operators::base_degrid_operator_2d<Vector<t_complex>>(
        u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, ft_plan, false, 1, 1,
        absolute_error, relative_error, dde_type::wkernel_radial);
    const auto& forward_expected_op = std::get<0>(measure_op);
    const auto& adjoint_expected_op = std::get<1>(measure_op);
    SECTION("Forward") {
      const Vector<t_complex> input = Vector<t_complex>::Ones(
          imsizex * imsizey * oversample_ratio_image_domain * oversample_ratio_image_domain);
      Vector<t_complex> output;
      direct(output, input);
      Vector<t_complex> output_expected;
      forward_expected_op(output_expected, input);
      CHECK(output.size() == 1);
      CHECK(std::imag(output(0)) == Approx(0.).margin(1e-3));
      CAPTURE(output_expected.head(1));
      CAPTURE(output.head(1));
      CAPTURE(1. / output.head(1).array());
      CAPTURE(1. / output_expected.head(1).array());
      CHECK(output.isApprox(output_expected, 1e-6));
    }
    SECTION("adjoint") {
      const Vector<t_complex> input = Vector<t_complex>::Ones(1);
      Vector<t_complex> output;
      indirect(output, input);
      CAPTURE(output.head(5));
      CAPTURE(input);
      Vector<t_complex> output_expected;
      adjoint_expected_op(output_expected, input);
      CAPTURE(output_expected.head(5));
      CAPTURE(1. / output.head(5).array());
      CAPTURE(1. / output_expected.head(5).array());
      CHECK(output.isApprox(output_expected, 1e-6));
    }
  }
}
