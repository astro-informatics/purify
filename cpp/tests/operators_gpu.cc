#include "purify/operators_gpu.h"
#include "purify/config.h"
#include "purify/types.h"
#include <arrayfire.h>
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/wproj_operators_gpu.h"
#include <sopt/power_method.h>
using namespace purify;
using namespace purify::notinstalled;
TEST_CASE("GPU Operators") {
  af::setDevice(0);
  af::info();
  // sopt::logging::set_level("debug");
  // purify::logging::set_level("debug");
  const t_uint M = 1000;
  const t_real oversample_ratio = 2;
  const t_uint imsizex = 128;
  const t_uint imsizey = 128;
  const t_uint N = imsizex * imsizey;
  const t_uint ftsizev = std::floor(imsizey * oversample_ratio);
  const t_uint ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 4;
  const t_uint Jv = 4;
  const t_uint power_iters = 100;
  const t_real power_tol = 1e-9;
  const kernels::kernel kernel = kernels::kernel::kb;
  const std::string& weighting_type = "natural";
  const t_real R = 0;
  auto u = Vector<t_real>::Random(M);
  auto v = Vector<t_real>::Random(M);
  utilities::vis_params uv_vis;
  uv_vis.u = u;
  uv_vis.v = v;
  uv_vis.w = u * 0.;
  uv_vis.weights = Vector<t_complex>::Random(M);
  uv_vis.vis = Vector<t_complex>::Random(M);
  uv_vis.units = utilities::vis_units::pixels;
  std::function<t_real(t_real)> kbu, kbv, ftkbu, ftkbv;
  std::tie(kbu, kbv, ftkbu, ftkbv) =
      create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  SECTION("FFT") {
    const Vector<t_complex> input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> output;
    Vector<t_complex> new_input;
    sopt::OperatorFunction<af::array> direct_gpu_fft, indirect_gpu_fft;
    std::tie(direct_gpu_fft, indirect_gpu_fft) =
        gpu::operators::init_af_FFT_2d(imsizey, imsizex, oversample_ratio);
    sopt::OperatorFunction<Vector<t_complex>> fft =
        gpu::host_wrapper(direct_gpu_fft, input.size(), input.size());
    sopt::OperatorFunction<Vector<t_complex>> ifft =
        gpu::host_wrapper(indirect_gpu_fft, input.size(), input.size());
    fft(output, input);
    ifft(new_input, output);
    CAPTURE(input.transpose().array() / new_input.transpose().array());
    CHECK(input.isApprox(new_input, 1e-6));
    sopt::OperatorFunction<Vector<t_complex>> direct_fft, indirect_fft;
    std::tie(direct_fft, indirect_fft) =
        operators::init_FFT_2d<Vector<t_complex>>(imsizey, imsizex, oversample_ratio);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> output_new;
    Vector<t_complex> output_old;
    fft(output_new, direct_input);
    direct_fft(output_old, direct_input);
    CHECK(output_old.isApprox(output_new, 1e-6));
    ifft(output_new, indirect_input);
    indirect_fft(output_old, indirect_input);
    CHECK(output_old.isApprox(output_new, 1e-6));
  }
  SECTION("Gridding") {
    sopt::OperatorFunction<af::array> direct_gpu_G, indirect_gpu_G;
    std::tie(direct_gpu_G, indirect_gpu_G) = gpu::operators::init_af_gridding_matrix_2d(
        uv_vis.u, uv_vis.v, Vector<t_complex>::Constant(M, 1.), imsizey, imsizex, oversample_ratio,
        kbu, kbv, Ju, Jv);
    const sopt::OperatorFunction<Vector<t_complex>> gpu_direct_G =
        gpu::host_wrapper(direct_gpu_G, ftsizeu * ftsizev, M);
    const sopt::OperatorFunction<Vector<t_complex>> gpu_indirect_G =
        gpu::host_wrapper(indirect_gpu_G, M, ftsizeu * ftsizev);
    sopt::OperatorFunction<Vector<t_complex>> direct_G, indirect_G;
    std::tie(direct_G, indirect_G) = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, Vector<t_complex>::Constant(M, 1.), imsizey, imsizex, oversample_ratio,
        kbv, kbu, Ju, Jv);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M);
    Vector<t_complex> output_new;
    Vector<t_complex> output_old;
    gpu_direct_G(output_new, direct_input);
    direct_G(output_old, direct_input);
    CHECK(output_new.isApprox(output_old, 1e-6));
    gpu_indirect_G(output_new, indirect_input);
    indirect_G(output_old, indirect_input);
    CHECK(output_new.isApprox(output_old, 1e-6));
  }
  SECTION("Zero Padding") {
    const Image<t_complex> S =
        details::init_correction2d(oversample_ratio, imsizey, imsizex, ftkbu, ftkbv, 0, 1, 1);
    CHECK(imsizex == S.cols());
    CHECK(imsizey == S.rows());
    sopt::OperatorFunction<Vector<t_complex>> directZ, indirectZ;
    std::tie(directZ, indirectZ) =
        operators::init_zero_padding_2d<Vector<t_complex>>(S, oversample_ratio);
    sopt::OperatorFunction<af::array> directZgpu, indirectZgpu;
    std::tie(directZgpu, indirectZgpu) =
        gpu::operators::init_af_zero_padding_2d(S.cast<t_complexf>(), oversample_ratio);
    sopt::OperatorFunction<Vector<t_complex>> directZ_gpu =
        gpu::host_wrapper(directZgpu, imsizex * imsizey, ftsizeu * ftsizev);
    sopt::OperatorFunction<Vector<t_complex>> indirectZ_gpu =
        gpu::host_wrapper(indirectZgpu, ftsizeu * ftsizev, imsizex * imsizey);
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    Vector<t_complex> direct_output_old;
    Vector<t_complex> direct_output_new;
    directZ(direct_output_old, direct_input);
    directZ_gpu(direct_output_new, direct_input);
    CHECK(direct_output_new.size() == ftsizeu * ftsizev);
    CHECK(direct_output_old.isApprox(direct_output_new, 1e-6));
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(ftsizeu * ftsizev);
    Vector<t_complex> indirect_output_old;
    Vector<t_complex> indirect_output_new;
    indirectZ(indirect_output_old, indirect_input);
    indirectZ_gpu(indirect_output_new, indirect_input);
    CHECK(indirect_output_new.size() == imsizex * imsizey);
    CHECK(indirect_output_old.isApprox(indirect_output_new, 1e-6));
  }
  SECTION("Serial Operator") {
    const auto measure_op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
        measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
            uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
            kernel, Ju, Jv),
        power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey)));

    const auto measure_op_gpu = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
            gpu::measurementoperator::init_degrid_operator_2d(uv_vis.u, uv_vis.v, uv_vis.w,
                                                              uv_vis.weights, imsizey, imsizex,
                                                              oversample_ratio, kernel, Ju, Jv),
            power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey)));
    const Vector<t_complex> direct_input = Vector<t_complex>::Random(imsizex * imsizey);
    const Vector<t_complex> direct_output = *measure_op_gpu * direct_input;
    CHECK(direct_output.size() == M);
    const Vector<t_complex> indirect_input = Vector<t_complex>::Random(M);
    const Vector<t_complex> indirect_output = measure_op_gpu->adjoint() * indirect_input;
    CHECK(indirect_output.size() == imsizex * imsizey);
    SECTION("Power Method") {
      auto op_norm = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
          *measure_op, power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey)));
      CHECK(std::abs(op_norm - 1.) < power_tol);
    }
    SECTION("Degrid") {
      const Vector<t_complex> input = Vector<t_complex>::Random(imsizex * imsizey);
      const Vector<t_complex> expected_output = *measure_op * input;
      const Vector<t_complex> actual_output = *measure_op_gpu * input;
      CHECK(expected_output.size() == actual_output.size());
      CHECK(actual_output.isApprox(expected_output, 1e-4));
    }
    SECTION("Grid") {
      const Vector<t_complex> input = Vector<t_complex>::Random(M);
      const Vector<t_complex> expected_output = measure_op->adjoint() * input;
      const Vector<t_complex> actual_output = measure_op_gpu->adjoint() * input;
      CHECK(expected_output.size() == actual_output.size());
      CHECK(actual_output.isApprox(expected_output, 1e-4));
    }
  }
  }
};
