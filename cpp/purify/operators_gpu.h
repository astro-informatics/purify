#ifndef PURIFY_OPERATORS_GPU_H
#define PURIFY_OPERATORS_GPU_H

#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include <tuple>
#include "purify/logging.h"
#include "purify/operators.h"
#include <sopt/chained_operators.h>
#include <sopt/linear_transform.h>
#include <sopt/power_method.h>
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
  for (t_int k = 0; k < G.outerSize(); ++k) {
    rows(k) = index;
    for (Sparse<t_complex>::InnerIterator it(G, k); it; ++it) {
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

sopt::OperatorFunction<Vector<t_complexf>> host_wrapper_float(
    const sopt::OperatorFunction<af::array> &gpu_operator, const t_uint &input_size,
    const t_uint &output_size, const t_uint &idx = 0) {
  return [=](Vector<t_complexf> &output, const Vector<t_complexf> &input) {
    af::setDevice(idx);
    const af::array gpu_input =
        af::array(input_size, reinterpret_cast<const af::cfloat *>(input.data()));
    af::array gpu_output;
    gpu_operator(gpu_output, gpu_input);
    const std::shared_ptr<t_complexf> gpu_data =
        std::shared_ptr<t_complexf>(reinterpret_cast<t_complexf *>(gpu_output.host<af::cfloat>()),
                                    [](const t_complexf *ptr) { delete[] ptr; });
    output = Vector<t_complexf>::Zero(output_size);
    std::copy(gpu_data.get(), gpu_data.get() + output_size, output.data());
  };
}
sopt::OperatorFunction<Vector<t_complex>> host_wrapper(
    const sopt::OperatorFunction<af::array> &gpu_operator, const t_uint &input_size,
    const t_uint &output_size, const t_uint &idx = 0) {
  const sopt::OperatorFunction<Vector<t_complexf>> float_wrapper =
      host_wrapper_float(gpu_operator, input_size, output_size, idx);
  return [=](Vector<t_complex> &output, const Vector<t_complex> &input) {
    Vector<t_complexf> output_float;
    float_wrapper(output_float, input.cast<t_complexf>());
    output = output_float.cast<t_complex>();
  };
}
namespace operators {
std::tuple<sopt::OperatorFunction<af::array>, sopt::OperatorFunction<af::array>>
init_af_zero_padding_2d(const Image<t_complexf> &S, const t_real &oversample_ratio) {
  const t_uint imsizex_ = S.cols();
  const t_uint imsizey_ = S.rows();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
  const af::array correction =
      af::array(imsizey_ * imsizex_, reinterpret_cast<const af::cfloat *>(S.data()));
  auto direct = [=](af::array &output, const af::array &x) {
    output = af::constant(0., ftsizev_, ftsizeu_, c32);
    output(af::seq(y_start, y_start + imsizey_ - 1), af::seq(x_start, x_start + imsizex_ - 1)) =
        af::moddims(correction * x, imsizey_, imsizex_);
    output = af::flat(output);
  };
  auto indirect = [=](af::array &output, const af::array &x) {
    output = af::moddims(x, ftsizev_, ftsizeu_)(af::seq(y_start, y_start + imsizey_ - 1),
                                                af::seq(x_start, x_start + imsizex_ - 1));
    output = correction * af::flat(output);
  };
  return std::make_tuple(direct, indirect);
}
std::tuple<sopt::OperatorFunction<af::array>, sopt::OperatorFunction<af::array>>
init_af_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                           const Vector<t_complex> weights,
                           const t_uint &imsizey_, const t_uint &imsizex_,
                           const t_real oversample_ratio,
                           const std::function<t_real(t_real)> kernelu,
                           const std::function<t_real(t_real)> kernelv,
                           const t_uint Ju = 4, const t_uint Jv = 4) {
  const Sparse<t_complex> interpolation_matrix =
      details::init_gridding_matrix_2d(u, v, weights, imsizey_, imsizex_, oversample_ratio,
                                       kernelu, kernelv, Ju, Jv);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();
  const std::shared_ptr<const af::array> G =
      std::make_shared<const af::array>(gpu::init_af_G_2d(interpolation_matrix));
  const std::shared_ptr<const af::array> G_adjoint =
      std::make_shared<const af::array>(gpu::init_af_G_2d(adjoint));
  auto direct = [G](af::array &output, const af::array &input) { output = af::matmul(*G, input); };
  auto indirect = [G_adjoint](af::array &output, const af::array &input) {
    output = af::matmul(*G_adjoint, input);
  };
  return std::make_tuple(direct, indirect);
}
#ifdef PURIFY_MPI
//! Constructs degridding operator with mpi and arrayfire
std::tuple<sopt::OperatorFunction<Vector<t_complex>>, sopt::OperatorFunction<Vector<t_complex>>>
init_af_gridding_matrix_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                           const Vector<t_real> &v, 
                           const Vector<t_complex> &weights, const t_uint &imsizey_,
                           const t_uint &imsizex_, const t_real oversample_ratio,
                           const std::function<t_real(t_real)> kernelu,
                           const std::function<t_real(t_real)> kernelv,
                           const t_uint Ju = 4, const t_uint Jv = 4) {
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_LOW_LOG("Constructing MPI Gridding Operator: G");
  Sparse<t_complex> interpolation_matrix =
      details::init_gridding_matrix_2d(u, v, weights, imsizey_, imsizex_, oversample_ratio,
                                       kernelu, kernelv, Ju, Jv);
  const DistributeSparseVector distributor(interpolation_matrix, comm);
  interpolation_matrix = purify::compress_outer(interpolation_matrix);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();
  const std::shared_ptr<const af::array> G =
      std::make_shared<const af::array>(gpu::init_af_G_2d(interpolation_matrix));
  const std::shared_ptr<const af::array> G_adjoint =
      std::make_shared<const af::array>(gpu::init_af_G_2d(adjoint));
  const sopt::OperatorFunction<af::array> directGgpu =
      [G](af::array &output, const af::array &input) { output = af::matmul(*G, input); };
  const sopt::OperatorFunction<af::array> indirectGgpu = [G_adjoint](af::array &output,
                                                                     const af::array &input) {
    output = af::matmul(*G_adjoint, input);
  };
  const sopt::OperatorFunction<Vector<t_complex>> directGhost =
      gpu::host_wrapper(directGgpu, interpolation_matrix.cols(), interpolation_matrix.rows());
  const sopt::OperatorFunction<Vector<t_complex>> indirectGhost =
      gpu::host_wrapper(indirectGgpu, interpolation_matrix.rows(), interpolation_matrix.cols());
  return std::make_tuple(
      [=](Vector<t_complex> &output, const Vector<t_complex> &input) {
        Vector<t_complex> dist_input;
        if (comm.is_root()) {
          assert(input.size() > 0);
          distributor.scatter(input, dist_input);
        } else {
          distributor.scatter(dist_input);
        }
        directGhost(output, dist_input);
      },
      [=](Vector<t_complex> &output, const Vector<t_complex> &input) {
        Vector<t_complex> dist_output;
        indirectGhost(dist_output, input);
        if (not comm.is_root()) {
          distributor.gather(dist_output);
        } else {
          distributor.gather(dist_output, output);
          assert(output.size() > 0);
        }
      });
}
#endif

std::tuple<sopt::OperatorFunction<af::array>, sopt::OperatorFunction<af::array>> init_af_FFT_2d(
    const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_) {
  auto const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  auto const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  auto direct = [=](af::array &output, const af::array &input) {
    output = af::fft2(af::moddims(input, ftsizev_, ftsizeu_), ftsizev_, ftsizeu_);
    output = af::flat(output) / std::sqrt(ftsizeu_ * ftsizev_);
  };
  auto indirect = [=](af::array &output, const af::array &input) {
    output = af::ifft2(af::moddims(input, ftsizev_, ftsizeu_), ftsizev_, ftsizeu_);
    output = af::flat(output) * std::sqrt(ftsizeu_ * ftsizev_);
  };
  return std::make_tuple(direct, indirect);
}
std::tuple<sopt::OperatorFunction<af::array>, sopt::OperatorFunction<af::array>>
af_base_padding_and_FFT_2d(const std::function<t_real(t_real)> &ftkernelu,
                           const std::function<t_real(t_real)> &ftkernelv, const t_uint &imsizey,
                           const t_uint &imsizex, const t_real oversample_ratio = 2,
                           const t_real w_mean = 0, const t_real &cellx = 1,
                           const t_real &celly = 1) {
  sopt::OperatorFunction<af::array> directZ, indirectZ;
  sopt::OperatorFunction<af::array> directFFT, indirectFFT;
  const Image<t_complex> S = purify::details::init_correction2d(
      oversample_ratio, imsizey, imsizex, ftkernelu, ftkernelv, w_mean, cellx, celly);
  PURIFY_LOW_LOG("Building GPU Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG("Constructing Zero Padding and Correction Operator: ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  std::tie(directZ, indirectZ) =
      purify::gpu::operators::init_af_zero_padding_2d(S.cast<t_complexf>(), oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  std::tie(directFFT, indirectFFT) =
      purify::gpu::operators::init_af_FFT_2d(imsizey, imsizex, oversample_ratio);
  const auto direct = sopt::chained_operators<af::array>(directFFT, directZ);
  const auto indirect = sopt::chained_operators<af::array>(indirectZ, indirectFFT);
  return std::make_tuple(direct, indirect);
}
std::tuple<sopt::OperatorFunction<Vector<t_complex>>, sopt::OperatorFunction<Vector<t_complex>>>
base_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real oversample_ratio = 2,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4, const bool w_term = false, 
                        const t_real cellx = 1, const t_real &celly = 1, const t_uint &idx = 0) {
  af::setDevice(idx);
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<af::array> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = af_base_padding_and_FFT_2d(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, w.mean(), cellx, celly);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  sopt::OperatorFunction<af::array> directG, indirectG;
  std::tie(directG, indirectG) = purify::gpu::operators::init_af_gridding_matrix_2d(
      u, v, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv, Ju, Jv);
  auto direct = gpu::host_wrapper(sopt::chained_operators<af::array>(directG, directFZ),
                                  imsizey * imsizex, u.size(), idx);
  auto indirect = gpu::host_wrapper(sopt::chained_operators<af::array>(indirectFZ, indirectG),
                                    u.size(), imsizex * imsizey, idx);
  return std::make_tuple(direct, indirect);
}

#ifdef PURIFY_MPI
std::tuple<sopt::OperatorFunction<Vector<t_complex>>, sopt::OperatorFunction<Vector<t_complex>>>
base_mpi_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const Vector<t_real> &w,
                            const Vector<t_complex> &weights, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real &oversample_ratio = 2,
                            const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                            const t_uint Jv = 4, const bool w_term = false,
                            const t_real cellx = 1, const t_real &celly = 1,
                            const t_uint &idx = 0) {
  af::setDevice(idx);
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<af::array> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) =
      af_base_padding_and_FFT_2d(ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  sopt::OperatorFunction<Vector<t_complex>> directG, indirectG;
  std::tie(directG, indirectG) = operators::init_af_gridding_matrix_2d(
      comm, u, v, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv, Ju, Jv);
  sopt::OperatorFunction<Vector<t_complex>> direct =
      gpu::host_wrapper(directFZ, imsizey * imsizex,
                        std::floor(imsizex * imsizey * oversample_ratio * oversample_ratio), idx);
  sopt::OperatorFunction<Vector<t_complex>> indirect = gpu::host_wrapper(
      indirectFZ, std::floor(imsizex * imsizey * oversample_ratio * oversample_ratio),
      imsizex * imsizey, idx);
  direct = sopt::chained_operators<Vector<t_complex>>(directG, direct);
  indirect = sopt::chained_operators<Vector<t_complex>>(indirect, indirectG);
  if (comm.is_root())
    return std::make_tuple(direct, indirect);
  else
    return std::make_tuple(directG, indirectG);
}
#endif
}  // namespace operators

namespace measurementoperator {

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, 
    const t_real cellx = 1, const t_real &celly = 1, const t_uint &idx = 0) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::gpu::operators::base_degrid_operator_2d(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, w_term, idx);
  auto direct = directDegrid;
  auto indirect = indirectDegrid;
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, M, indirect, N}, power_iters, power_tol,
      Vector<t_complex>::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm);
  direct = sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
  indirect = sopt::chained_operators<Vector<t_complex>>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(direct, M, indirect, N);
}
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d(
    const utilities::vis_params &uv_vis_input, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &cell_x, const t_real &cell_y, const t_real &oversample_ratio = 2,
    const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_term = false, const t_uint &idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::measurementoperator::init_degrid_operator_2d(
      uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio, power_iters,
      power_tol, kernel, Ju, Jv, w_term, cell_x, cell_y, idx);
}

#ifdef PURIFY_MPI
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, 
    const t_real cellx = 1, const t_real &celly = 1, const t_uint &idx = 0) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::gpu::operators::base_mpi_degrid_operator_2d(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, w_term, idx);
  auto Broadcast = purify::operators::init_broadcaster<Vector<t_complex>>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<Vector<t_complex>>(Broadcast, indirectDegrid);
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, M, indirect, N}, power_iters, power_tol,
      comm.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(imsizex * imsizey)));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm);
  direct = sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
  indirect = sopt::chained_operators<Vector<t_complex>>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(direct, M, indirect, N);
}
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, 
    const t_uint &idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::measurementoperator::init_degrid_operator_2d_mpi(
      comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
      power_iters, power_tol, kernel, Ju, Jv, w_term, cell_x, cell_y, idx);
}
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, 
    const t_real cellx = 1, const t_real &celly = 1, const t_uint &idx = 0) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::gpu::operators::base_degrid_operator_2d(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, w_term, cellx,
      celly, idx);
  const auto allsumall = purify::operators::init_all_sum_all<Vector<t_complex>>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<Vector<t_complex>>(allsumall, indirectDegrid);
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, M, indirect, N}, power_iters, power_tol,
      Vector<t_complex>::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm);
  direct = sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
  indirect = sopt::chained_operators<Vector<t_complex>>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(direct, M, indirect, N);
}
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>> const> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, 
    const t_uint &idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::measurementoperator::init_degrid_operator_2d(
      comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
      power_iters, power_tol, kernel, Ju, Jv, w_term, idx);
}
#endif
}  // namespace measurementoperator
}  // namespace gpu
};  // namespace purify
#endif
#endif
