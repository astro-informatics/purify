#ifndef PURIFY_WPROJ_OPERATORS_GPU_H
#define PURIFY_WPROJ_OPERATORS_GPU_H

#include "purify/operators_gpu.h"
#include "purify/wproj_operators.h"

#ifdef PURIFY_ARRAYFIRE
namespace purify {
namespace gpu {
namespace operators {
std::tuple<sopt::OperatorFunction<af::array>, sopt::OperatorFunction<af::array>>
af_base_padding_and_FFT_2d(const std::function<t_real(t_real)> &ftkerneluv, const t_uint imsizey,
                           const t_uint imsizex, const t_real oversample_ratio, const t_real w_mean,
                           const t_real cellx, const t_real celly) {
  sopt::OperatorFunction<af::array> directZ, indirectZ;
  sopt::OperatorFunction<af::array> directFFT, indirectFFT;
  const Image<t_complex> S = purify::details::init_correction_radial_2d(
      oversample_ratio, imsizey, imsizex, ftkerneluv, w_mean, cellx, celly);
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
                        const t_uint &imsizex, const t_real oversample_ratio,
                        const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
                        const bool w_stacking, const t_real cellx, const t_real celly,
                        const t_real absolute_error, const t_real relative_error,
                        const dde_type dde, const t_uint idx = 0) {
  sopt::OperatorFunction<af::array> directFZ, indirectFZ;
  sopt::OperatorFunction<af::array> directG, indirectG;
  t_real const w_mean = w_stacking ? w.array().mean() : 0;
  switch (dde) {
  case (dde_type::wkernel_radial): {
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directFZ, indirectFZ) = af_base_padding_and_FFT_2d(
        std::get<0>(kerneluvs), imsizey, imsizex, oversample_ratio, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    PURIFY_MEDIUM_LOG("Mean, w: {}, +/- {}", w.array().mean(), (w.maxCoeff() - w.minCoeff()) * 0.5);
    std::tie(directG, indirectG) =
        init_af_gridding_matrix_2d(u, v, w.array() - w_mean, weights, imsizey, imsizex,
                                   oversample_ratio, std::get<0>(kerneluvs), std::get<1>(kerneluvs),
                                   Ju, Jw, cellx, celly, absolute_error, relative_error, dde);
    break;
  }
  case (dde_type::wkernel_2d): {
    std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
    std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
        purify::create_kernels(kernel, Ju, Ju, imsizey, imsizex, oversample_ratio);
    std::tie(directFZ, indirectFZ) = af_base_padding_and_FFT_2d(
        ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    PURIFY_MEDIUM_LOG("Mean, w: {}, +/- {}", w.array().mean(), (w.maxCoeff() - w.minCoeff()) * 0.5);
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directG, indirectG) =
        init_af_gridding_matrix_2d(u, v, w.array() - w_mean, weights, imsizey, imsizex,
                                   oversample_ratio, std::get<0>(kerneluvs), std::get<1>(kerneluvs),
                                   Ju, Jw, cellx, celly, absolute_error, relative_error, dde);
    break;
  }
  default:
    throw std::runtime_error("DDE is not recognised.");
  }
  auto direct = gpu::host_wrapper(sopt::chained_operators<af::array>(directG, directFZ),
                                  imsizey * imsizex, u.size(), idx);
  auto indirect = gpu::host_wrapper(sopt::chained_operators<af::array>(indirectFZ, indirectG),
                                    u.size(), imsizex * imsizey, idx);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  return std::make_tuple(direct, indirect);
}
}  // namespace operators

namespace measurementoperator {

//! Returns linear transform that is the standard degridding operator
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint imsizey, const t_uint imsizex,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error,
    const t_real relative_error, const dde_type dde, const t_uint idx = 0) {
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = gpu::operators::base_degrid_operator_2d(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cellx,
      celly, absolute_error, relative_error, dde, idx);
  auto direct = directDegrid;
  auto indirect = indirectDegrid;
  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(direct, M, indirect, N);
}

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d(
    const utilities::vis_params &uv_vis_input, const t_uint imsizey, const t_uint imsizex,
    const t_real cell_x, const t_real cell_y, const t_real oversample_ratio,
    const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking,
    const t_real absolute_error, const t_real relative_error, const dde_type dde,
    const t_uint idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                 oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x, cell_y,
                                 absolute_error, relative_error, dde, idx);
}
#ifdef PURIFY_MPI
//! Returns linear transform that is the weighted degridding operator with mpi all sum all
std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde,
    const t_uint idx = 0) {
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<Vector<t_complex>> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = gpu::operators::base_degrid_operator_2d(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cellx,
      celly, absolute_error, relative_error, dde, idx);
  const auto allsumall = purify::operators::init_all_sum_all<Vector<t_complex>>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<Vector<t_complex>>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(direct, M, indirect, N);
}

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real absolute_error, const t_real relative_error,
    const dde_type dde, const t_uint idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                 imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x,
                                 cell_y, absolute_error, relative_error, dde, idx);
}

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde,
    const t_uint idx = 0) {
  throw std::runtime_error("Under construction!");
  return init_degrid_operator_2d(comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel,
                                 Ju, Jw, w_stacking, cellx, celly, absolute_error, relative_error,
                                 dde, idx);
}

std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real absolute_error, const t_real relative_error,
    const dde_type dde, const t_uint idx = 0) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                 imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x,
                                 cell_y, absolute_error, relative_error, dde, idx);
}
#endif
}  // namespace measurementoperator
}  // namespace gpu
}  // namespace purify
#endif
#endif
