#ifndef PURIFY_COMPACT_OPERATORS_GPU
#define PURIFY_COMPACT_OPERATORS_GPU

#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include <tuple>
#include "purify/logging.h"
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#ifdef PURIFY_ARRAYFIRE
#include <arrayfire.h>
#include "purify/operators_gpu.h"
#endif

#ifdef PURIFY_ARRAYFIRE
namespace purify {
namespace gpu {
namespace details {
sopt::OperatorFunction<af::array> init_af_gridding_degridding_matrix_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> weights, const t_uint &imsizey_, const t_uint &imsizex_,
    const t_real oversample_ratio, const std::function<t_real(t_real)> kernelu,
    const std::function<t_real(t_real)> kernelv,
    const std::function<t_complex(t_real, t_real, t_real)> kernelw, const t_uint Ju = 4,
    const t_uint Jv = 4, const t_uint Jw = 6, const bool w_term = false) {
  const Sparse<t_complex> interpolation_matrix = purify::details::init_gridding_matrix_2d(
      u, v, w, weights, imsizey_, imsizex_, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv, Jw,
      w_term);
  Sparse<t_complex> gTg = interpolation_matrix.adjoint() * interpolation_matrix;
  PURIFY_LOW_LOG("G non zeros: {}", interpolation_matrix.nonZeros());
  gTg.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
    return std::abs(value) > 1e-12;
  });
  PURIFY_LOW_LOG("GTG non zeros: {}", gTg.nonZeros());
  const af::array GTG = gpu::init_af_G_2d(gTg);
  return [GTG](af::array &output, const af::array &input) { output = af::matmul(GTG, input); };
}
#ifdef PURIFY_MPI
//! Constructs degridding operator with mpi and arrayfire
sopt::OperatorFunction<Vector<t_complex>> init_af_gridding_degridding_matrix_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey_,
    const t_uint &imsizex_, const t_real oversample_ratio,
    const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv,
    const std::function<t_complex(t_real, t_real, t_real)> kernelw, const t_uint Ju = 4,
    const t_uint Jv = 4, const t_uint Jw = 6, const bool w_term = false) {
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_LOW_LOG("Constructing MPI Gridding Operator: G");
  Sparse<t_complex> interpolation_matrix = purify::details::init_gridding_matrix_2d(
      u, v, w, weights, imsizey_, imsizex_, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv, Jw,
      w_term);
  const DistributeSparseVector distributor(interpolation_matrix, comm);
  interpolation_matrix = purify::compress_outer(interpolation_matrix);
  PURIFY_LOW_LOG("G non zeros: {}", interpolation_matrix.nonZeros());
  Sparse<t_complex> gTg = interpolation_matrix.adjoint() * interpolation_matrix;
  gTg.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
    return std::abs(value) > 1e-12;
  });
  PURIFY_LOW_LOG("GTG non zeros: {}", gTg.nonZeros());
  const af::array GTG = gpu::init_af_G_2d(gTg);
  const sopt::OperatorFunction<af::array> directGgpu =
      [GTG](af::array &output, const af::array &input) { output = af::matmul(GTG, input); };
  const sopt::OperatorFunction<Vector<t_complex>> directGhost =
      gpu::host_wrapper(directGgpu, gTg.cols(), gTg.rows());
  return [=](Vector<t_complex> &output, const Vector<t_complex> &input) {
    Vector<t_complex> dist_swap;
    if (comm.is_root()) {
      assert(input.size() > 0);
      distributor.scatter(input, dist_swap);
    } else {
      distributor.scatter(dist_swap);
    }
    Vector<t_complex> dist_out;
    directGhost(dist_out, dist_swap);
    if (not comm.is_root()) {
      distributor.gather(dist_out);
    } else {
      distributor.gather(dist_out, output);
      assert(output.size() > 0);
    }
  };
}
#endif
}  // namespace details

namespace operators {
sopt::OperatorFunction<Vector<t_complex>> base_grid_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6,
    const t_real cellx = 1, const t_real &celly = 1) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<af::array> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = gpu::operators::af_base_padding_and_FFT_2d(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::function<t_complex(t_real, t_real, t_real)> kernelw =
      projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                     oversample_ratio);
  const auto GTG = details::init_af_gridding_degridding_matrix_2d(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv, Jw,
      w_term);
  return gpu::host_wrapper(sopt::chained_operators<af::array>(indirectFZ, GTG, directFZ),
                           imsizey * imsizex, imsizey * imsizex);
}

#ifdef PURIFY_MPI
sopt::OperatorFunction<Vector<t_complex>> base_mpi_grid_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_term = false, const t_uint Jw = 6, const t_real cellx = 1,
    const t_real &celly = 1) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<af::array> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = gpu::operators::af_base_padding_and_FFT_2d(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::function<t_complex(t_real, t_real, t_real)> kernelw =
      projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                     oversample_ratio);
  sopt::OperatorFunction<Vector<t_complex>> gTg = details::init_af_gridding_degridding_matrix_2d(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv,
      Jw, w_term);
  sopt::OperatorFunction<Vector<t_complex>> direct =
      gpu::host_wrapper(directFZ, imsizey * imsizex,
                        std::floor(imsizex * imsizey * oversample_ratio * oversample_ratio));
  sopt::OperatorFunction<Vector<t_complex>> indirect = gpu::host_wrapper(
      indirectFZ, std::floor(imsizex * imsizey * oversample_ratio * oversample_ratio),
      imsizex * imsizey);
  if (comm.is_root())
    return sopt::chained_operators<Vector<t_complex>>(indirect, gTg, direct);
  else
    return gTg;
}
#endif
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6,
    const t_real cellx = 1, const t_real &celly = 1) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> direct =
      purify::gpu::operators::base_grid_degrid_operator_2d(
          u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, w_term, Jw);
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, N, [](Vector<t_complex> &out, const Vector<t_complex> &in) { out = in; }, N},
      power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm * op_norm);
  return sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
}
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d(
    const utilities::vis_params &uv_vis_input, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &cell_x, const t_real &cell_y, const t_real &oversample_ratio = 2,
    const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_term = false, const t_uint Jw = 6) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::operators::init_grid_degrid_operator_2d(
      uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio, power_iters,
      power_tol, kernel, Ju, Jv, w_term, Jw, cell_x, cell_y);
}

#ifdef PURIFY_MPI
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6,
    const t_real cellx = 1, const t_real &celly = 1) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> direct =
      purify::gpu::operators::base_mpi_grid_degrid_operator_2d(
          comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, w_term, Jw);
  auto Broadcast = purify::operators::init_broadcaster<Vector<t_complex>>(comm);
  direct = sopt::chained_operators<Vector<t_complex>>(Broadcast, direct);
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, N, [](Vector<t_complex> &out, const Vector<t_complex> &in) { out = in; }, N},
      power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm * op_norm);
  return sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
}
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::operators::init_grid_degrid_operator_2d_mpi(
      comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
      power_iters, power_tol, kernel, Ju, Jv, w_term, Jw, cell_x, cell_y);
}
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6,
    const t_real cellx = 1, const t_real &celly = 1) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> direct =
      purify::gpu::operators::base_grid_degrid_operator_2d(u, v, w, weights, imsizey, imsizex,
                                                           oversample_ratio, kernel, Ju, Jv, w_term,
                                                           Jw, cellx, celly);
  const auto allsumall = purify::operators::init_all_sum_all<Vector<t_complex>>(comm);
  direct = sopt::chained_operators<Vector<t_complex>>(allsumall, direct);
  const t_real op_norm = sopt::algorithm::power_method<Vector<t_complex>>(
      {direct, N, [](Vector<t_complex> &out, const Vector<t_complex> &in) { out = in; }, N},
      power_iters, power_tol, Vector<t_complex>::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<Vector<t_complex>>(op_norm * op_norm);
  return sopt::chained_operators<Vector<t_complex>>(direct, operator_norm);
}
sopt::OperatorFunction<Vector<t_complex>> init_grid_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_term = false, const t_uint Jw = 6) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return gpu::operators::init_grid_degrid_operator_2d(
      comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
      power_iters, power_tol, kernel, Ju, Jv, w_term, Jw);
}
#endif
}  // namespace operators
}  // namespace gpu
}  // namespace purify
#endif
#endif
