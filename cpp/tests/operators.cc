#include "purify/operators.h"
#include "purify/config.h"
#include <iostream>
#include "catch.hpp"
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/types.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("Operators") {
  // sopt::logging::set_level("debug");
  // purify::logging::set_level("debug");
  const t_uint M = 1000;
  const t_real oversample_ratio = 2;
  const t_uint imsizex = 128;
  const t_uint imsizey = 128;
  const t_uint ftsizev = std::floor(imsizey * oversample_ratio);
  const t_uint ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 4;
  const t_uint Jv = 4;
  const t_uint power_iters = 100;
  const t_real power_tol = 1e-9;
  const std::string &kernel = "kb";
  const std::string &ft_plan = "measure";
  const std::string &weighting_type = "natural";
  const t_real R = 0;
  auto u = Vector<t_real>::Random(M);
  auto v = Vector<t_real>::Random(M);
  utilities::vis_params uv_vis;
  uv_vis.u = u;
  uv_vis.v = v;
  uv_vis.w = u * 0.;
  uv_vis.weights = Vector<t_complex>::Random(M);
  uv_vis.vis = Vector<t_complex>::Random(M);
  uv_vis.units = "pixel";
  MeasurementOperator expected_op(uv_vis, Ju, Jv, kernel, imsizex, imsizey, power_iters,
                                  oversample_ratio);
  std::function<t_real(t_real)> kbu, kbv, ftkbu, ftkbv;
  std::tie(kbu, kbv, ftkbu, ftkbv)
      = create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  SECTION("Gridding") {
    sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
    std::tie(directG, indirectG) = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, uv_vis.w, Vector<t_complex>::Constant(M, 1.), imsizey, imsizex,
        oversample_ratio, kbv, kbu, Ju, Jv);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> direct_output;
    directG(direct_output, direct_input);
    CHECK(direct_output.size() == M);
    const Vector<t_complex> expected_direct = expected_op.G * direct_input;
    CHECK(direct_output.size() == expected_direct.size());
    REQUIRE(direct_output.isApprox(expected_direct, 1e-4));
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M);
    Vector<t_complex> indirect_output;
    indirectG(indirect_output, indirect_input);
    CHECK(indirect_output.size() == ftsizev * ftsizeu);
    const Vector<t_complex> expected_indirect = expected_op.G.adjoint() * indirect_input;
    REQUIRE(indirect_output.isApprox(expected_indirect, 1e-4));
  }
  SECTION("Zero Padding") {
    const Image<t_real> S
        = details::init_correction2d(oversample_ratio, imsizey, imsizex, ftkbu, ftkbv);
    CHECK(imsizex == S.cols());
    CHECK(imsizey == S.rows());
    CHECK(S.cols() == expected_op.S.cols());
    CHECK(S.rows() == expected_op.S.rows());
    INFO(S(0) / expected_op.S(0));
    INFO(S(5) / expected_op.S(5));
    REQUIRE(S.isApprox(expected_op.S, 1e-4));
    sopt::OperatorFunction<Vector<t_complex>> directZ, indirectZ;
    std::tie(directZ, indirectZ)
        = operators::init_zero_padding_2d<Vector<t_complex>>(S, oversample_ratio);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    Vector<t_complex> direct_output;
    directZ(direct_output, direct_input);
    CHECK(direct_output.size() == ftsizeu * ftsizev);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizeu * ftsizev);
    Vector<t_complex> indirect_output;
    indirectZ(indirect_output, indirect_input);
    CHECK(indirect_output.size() == imsizex * imsizey);
  }
  SECTION("FFT") {
    sopt::OperatorFunction<Vector<t_complex>> directFFT, indirectFFT;
    std::tie(directFFT, indirectFFT)
        = operators::init_FFT_2d<Vector<t_complex>>(imsizey, imsizex, oversample_ratio, ft_plan);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> direct_output;
    directFFT(direct_output, direct_input);
    CHECK(direct_output.size() == ftsizeu * ftsizev);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> indirect_output;
    indirectFFT(indirect_output, indirect_input);
    CHECK(indirect_output.size() == ftsizev * ftsizeu);
    Vector<t_complex> inverse_check;
    Vector<t_complex> buff;
    directFFT(buff, direct_input);
    indirectFFT(inverse_check, buff);
    CHECK(inverse_check.isApprox(direct_input, 1e-4));
  }
  SECTION("Weights") {
    MeasurementOperator weighted_expected_op(uv_vis, Ju, Jv, kernel, imsizex, imsizey, power_iters,
                                             oversample_ratio, 1, 1, "natural", 0);
    sopt::OperatorFunction<Vector<t_complex>> directW, indirectW;
    std::tie(directW, indirectW)
        = purify::operators::init_weights_<Vector<t_complex>>(uv_vis.weights);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(M);
    Vector<t_complex> direct_output;
    directW(direct_output, direct_input);
    CHECK(direct_output.size() == M);
    const Vector<t_complex> expected_direct = weighted_expected_op.W.array() * direct_input.array();
    CHECK(expected_direct.isApprox(direct_output, 1e-4));
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M);
    Vector<t_complex> indirect_output;
    indirectW(indirect_output, indirect_input);
    CHECK(indirect_output.size() == M);
    const Vector<t_complex> expected_indirect
        = weighted_expected_op.W.conjugate().array() * indirect_input.array();
    CHECK(expected_indirect.isApprox(indirect_output, 1e-4));
  }
  SECTION("Create Weighted Measurement Operator") {
    MeasurementOperator weighted_expected_op(uv_vis, Ju, Jv, kernel, imsizex, imsizey, power_iters,
                                             oversample_ratio, 1, 1, "natural");
    const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, Ju, Jv, ft_plan);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    const Vector<t_complex> direct_output = measure_op * direct_input;
    CHECK(direct_output.size() == M);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M);
    const Vector<t_complex> indirect_output = measure_op.adjoint() * indirect_input;
    CHECK(indirect_output.size() == imsizex * imsizey);
    SECTION("Power Method") {
      auto op_norm = details::power_method<Vector<t_complex>>(
          measure_op, power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey));
      CHECK(std::abs(op_norm - 1.) < power_tol);
    }
    SECTION("Degrid") {
      const Vector<t_complex> input = Vector<t_complex>::Random(imsizex * imsizey);
      const Image<t_complex> input_image = Image<t_complex>::Map(input.data(), imsizey, imsizex);
      const Vector<t_complex> expected_output = weighted_expected_op.degrid(input_image);
      const Vector<t_complex> actual_output = measure_op * input;
      CHECK(expected_output.size() == actual_output.size());
      CHECK(actual_output.isApprox(expected_output, 1e-4));
    }
    SECTION("Grid") {
      const Vector<t_complex> input = Vector<t_complex>::Random(M);
      const Vector<t_complex> expected_output
          = Image<t_complex>::Map(weighted_expected_op.grid(input).data(), imsizex * imsizey, 1);
      const Vector<t_complex> actual_output = measure_op.adjoint() * input;
      CHECK(expected_output.size() == actual_output.size());
      CHECK(actual_output.isApprox(expected_output, 1e-4));
    }
  }
}
