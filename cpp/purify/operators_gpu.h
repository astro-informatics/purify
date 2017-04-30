#ifndef PURIFY_OPERATORS_GPU_H
#define PURIFY_OPERATORS_GPU_H

#include "purify/config.h"
#include <iostream>
#include <tuple>
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"
#ifdef PURIFY_ARRAYFIRE
#include <arrayfire.h>
#endif

#ifdef PURIFY_ARRAYFIRE
namespace purify {

namespace gpu {

af::array init_af_G_2d(const Sparse<t_complex> &G) {
  Vector<t_complexf> data(G.nonZeros());
  Vector<t_int> rows(G.rows() + 1);
  Vector<t_int> cols(G.nonZeros());
  rows(G.rows()) = G.nonZeros();
  t_int index = 0;
  for(t_int k = 0; k < G.outerSize(); ++k) {
    rows(k) = index;
    for(Sparse<t_complex>::InnerIterator it(G, k); it; ++it) {
      data(index) = static_cast<t_complexf>(it.value());
      cols(index) = it.col();
      index++;
    }
  }
  assert(data.size() == G.nonZeros());
  return af::sparse(G.rows(), G.cols(), G.nonZeros(),
                    reinterpret_cast<const af::cfloat *>(data.data()), rows.data(), cols.data(),
                    c32);
}

namespace operators {
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_af_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const t_uint &imsizey_,
                           const t_uint &imsizex_, const t_real oversample_ratio,
                           const t_real resample_factor,
                           const std::function<t_real(t_real)> kernelu,
                           const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                           const t_uint Jv = 4) {
  const Sparse<t_complex> interpolation_matrix = details::init_gridding_matrix_2d(
      u, v, imsizey_, imsizex_, oversample_ratio, resample_factor, kernelu, kernelv, Ju, Jv);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();
  const af::array G = gpu::init_af_G_2d(interpolation_matrix);
  const af::array G_adjoint = gpu::init_af_G_2d(adjoint);
  auto direct = [G](T &output, const T &input) { output = af::matmul(G, input); };
  auto indirect = [G_adjoint](T &output, const T &input) { output = af::matmul(G_adjoint, input); };
  // af_print(G);
  return std::make_tuple(direct, indirect);
}
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_af_FFT_2d(const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_,
               const t_real &resample_factor_) {
  auto const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  auto const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  auto direct = [=](T &output, const T &input) {
    output = af::moddims(input, ftsizev_, ftsizeu_);
    output = af::fft2(output, ftsizev_, ftsizeu_);
    output = af::moddims(output, ftsizev_ * ftsizeu_, 1);
  };
  auto indirect = [=](T &output, const T &input) {
    output = af::moddims(input, ftsizev_, ftsizeu_);
    output = af::ifft2(output, ftsizev_, ftsizeu_);
    output = af::moddims(output, ftsizev_ * ftsizeu_, 1);
  };
  return std::make_tuple(direct, indirect);
}
sopt::OperatorFunction<Vector<t_complexf>>
host_wrapper_float(const sopt::OperatorFunction<af::array> &gpu_operator, const t_uint &input_size,
                   const t_uint &output_size) {
  return [=](Vector<t_complexf> &output, const Vector<t_complexf> &input) {
    const af::array gpu_input
        = af::array(input_size, reinterpret_cast<const af::cfloat *>(input.data()));
    af::array gpu_output;
    gpu_operator(gpu_output, gpu_input);
    const std::shared_ptr<t_complexf> gpu_data
        = std::shared_ptr<t_complexf>(reinterpret_cast<t_complexf *>(gpu_output.host<af::cfloat>()),
                                      [](const t_complexf *ptr) { delete[] ptr; });
    output = Vector<t_complexf>::Zero(output_size);
    std::copy(gpu_data.get(), gpu_data.get() + output_size, output.data());
  };
}
sopt::OperatorFunction<Vector<t_complex>>
host_wrapper(const sopt::OperatorFunction<af::array> &gpu_operator, const t_uint &input_size,
             const t_uint &output_size) {
  const sopt::OperatorFunction<Vector<t_complexf>> float_wrapper
      = host_wrapper_float(gpu_operator, input_size, output_size);
  return [=](Vector<t_complex> &output, const Vector<t_complex> &input) {
    Vector<t_complexf> output_float;
    float_wrapper(output_float, input.cast<t_complexf>());
    output = output_float.cast<t_complex>();
  };
}
} // namespace operators
} // namespace gpu
}; // namespace purify
#endif
#endif
