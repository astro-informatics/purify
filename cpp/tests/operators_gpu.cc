#include "purify/operators_gpu.h"
#include "purify/config.h"
#include <arrayfire.h>
#include <iostream>
#include "catch.hpp"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"
using namespace purify;
using namespace purify::notinstalled;
TEST_CASE("GPU Operators") {
  af::setDevice(0);
  af::info();
  sopt::logging::set_level("debug");
  purify::logging::set_level("debug");
  const t_uint M = 4;
  const t_real oversample_ratio = 2;
  const t_real resample_factor = 1;
  const t_uint imsizex = 2;
  const t_uint imsizey = 2;
  const t_uint ftsizev = std::floor(imsizey * oversample_ratio);
  const t_uint ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 4;
  const t_uint Jv = 4;
  const t_uint power_iters = 100;
  const t_real power_tol = 1e-9;
  const std::string &kernel = "kb";
  const std::string &ft_plan = "estimate";
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
  std::function<t_real(t_real)> kbu, kbv, ftkbu, ftkbv;
  std::tie(kbu, kbv, ftkbu, ftkbv)
      = create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  SECTION("FFT") {
    const Vector<t_complex> input = Vector<t_complex>::Random(ftsizev * ftsizeu);
    Vector<t_complex> output;
    Vector<t_complex> new_input;
    sopt::OperatorFunction<af::array> direct_gpu_fft, indirect_gpu_fft;
    std::tie(direct_gpu_fft, indirect_gpu_fft) = gpu::operators::init_af_FFT_2d<af::array>(
        imsizey, imsizex, oversample_ratio, resample_factor);
    sopt::OperatorFunction<Vector<t_complex>> fft
        = gpu::operators::host_wrapper(direct_gpu_fft, input.size(), input.size());
    sopt::OperatorFunction<Vector<t_complex>> ifft
        = gpu::operators::host_wrapper(indirect_gpu_fft, input.size(), input.size());
    fft(output, input);
    ifft(new_input, output);
    CHECK(input.isApprox(new_input, 1e-6));
    sopt::OperatorFunction<Vector<t_complex>> direct_fft, indirect_fft;
    std::tie(direct_fft, indirect_fft) = operators::init_FFT_2d<Vector<t_complex>>(
        imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
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
    std::tie(direct_gpu_G, indirect_gpu_G) = gpu::operators::init_af_gridding_matrix_2d<af::array>(
        uv_vis.u, uv_vis.v, imsizey, imsizex, oversample_ratio, resample_factor, kbu, kbv, Ju, Jv);
    const sopt::OperatorFunction<Vector<t_complex>> gpu_direct_G
        = gpu::operators::host_wrapper(direct_gpu_G, ftsizeu * ftsizev, M);
    const sopt::OperatorFunction<Vector<t_complex>> gpu_indirect_G
        = gpu::operators::host_wrapper(indirect_gpu_G, M, ftsizeu * ftsizev);
    sopt::OperatorFunction<Vector<t_complex>> direct_G, indirect_G;
    std::tie(direct_G, indirect_G) = operators::init_gridding_matrix_2d<Vector<t_complex>>(
        uv_vis.u, uv_vis.v, imsizey, imsizex, oversample_ratio, resample_factor, kbv, kbu, Ju, Jv);
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
}
