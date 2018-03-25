#ifndef PURIFY_COMPACT_OPERATORS_H
#define PURIFY_COMPACT_OPERATORS_H

#include "purify/config.h"
#include <iostream>
#include <tuple>
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#include "purify/mpi_utilities.h"
#endif

namespace purify {

namespace operators {

#ifdef PURIFY_MPI
//! Constructs a combined gridding/degridding operator using MPI
template <class T>
sopt::OperatorFunction<T>
init_gridding_degridding_matrix_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                                   const Vector<t_real> &v, const Vector<t_real> &w,
                                   const Vector<t_complex> &weights, const t_uint &imsizey_,
                                   const t_uint &imsizex_, const t_real oversample_ratio,
                                   const std::function<t_real(t_real)> kernelu,
                                   const std::function<t_real(t_real)> kernelv,
                                   const std::function<t_complex(t_real, t_real, t_real)> kernelw,
                                   const t_uint Ju = 4, const t_uint Jv = 4, const t_uint Jw = 6,
                                   const bool w_term = false, const t_real &cellx = 1,
                                   const t_real &celly = 1) {

  Sparse<t_complex> interpolation_matrix
      = details::init_gridding_matrix_2d(u, v, w, weights, imsizey_, imsizex_, oversample_ratio,
                                         kernelu, kernelv, kernelw, Ju, Jv, Jw, w_term);
  const DistributeSparseVector distributor(interpolation_matrix, comm);
  interpolation_matrix = purify::compress_outer(interpolation_matrix);
  PURIFY_LOW_LOG("G non zeros: {}", interpolation_matrix.nonZeros());
  Sparse<t_complex> GTG = interpolation_matrix.adjoint() * interpolation_matrix;
  GTG.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
    return std::abs(value) > 1e-12;
  });
  PURIFY_LOW_LOG("GTG non zeros: {}", GTG.nonZeros());

  return [=](T &output, const T &input) {
    if(comm.is_root()) {
      assert(input.size() > 0);
      distributor.scatter(input, output);
    } else {
      distributor.scatter(output);
    }
    output = utilities::sparse_multiply_matrix(GTG, output);
    if(not comm.is_root()) {
      distributor.gather(output);
    } else {
      distributor.gather(output, output);
    }
  };
}
template <class T>
sopt::OperatorFunction<T>
base_mpi_grid_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                                 const Vector<t_real> &v, const Vector<t_real> &w,
                                 const Vector<t_complex> &weights, const t_uint &imsizey,
                                 const t_uint &imsizex, const t_real oversample_ratio = 2,
                                 const kernels::kernel kernel = kernels::kernel::kb,
                                 const t_uint Ju = 4, const t_uint Jv = 4,
                                 const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                                 const bool w_term = false, const t_uint Jw = 6,
                                 const t_real &cellx = 1, const t_real &celly = 1) {

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, 0., cellx, celly);
  sopt::OperatorFunction<T> GTG;
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::function<t_complex(t_real, t_real, t_real)> kernelw
      = projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                       oversample_ratio);
  GTG = purify::operators::init_gridding_degridding_matrix_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelu, kernelv, kernelw, Ju, Jv,
      Jw, w_term, cellx, celly);
  if(comm.is_root())
    return sopt::chained_operators<T>(indirectFZ, GTG, directFZ);
  else
    return GTG;
}
#endif

//! Constructs a combined gridding and degridding operator
template <class T>
sopt::OperatorFunction<T>
init_gridding_degridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                   const Vector<t_real> &w, const Vector<t_complex> &weights,
                                   const t_uint &imsizey_, const t_uint &imsizex_,
                                   const t_uint &oversample_ratio,
                                   const std::function<t_real(t_real)> kernelu,
                                   const std::function<t_real(t_real)> kernelv,
                                   std::function<t_complex(t_real, t_real, t_real)> kernelw,
                                   const t_uint Ju = 4, const t_uint Jv = 4, const t_uint Jw = 6,
                                   const bool w_term = false, const t_real &cellx = 1,
                                   const t_real &celly = 1) {

  const Sparse<t_complex> interpolation_matrix
      = details::init_gridding_matrix_2d(u, v, w, weights, imsizey_, imsizex_, oversample_ratio,
                                         kernelu, kernelv, kernelw, Ju, Jv, Jw, w_term);
  PURIFY_LOW_LOG("G non zeros: {}", interpolation_matrix.nonZeros());
  Sparse<t_complex> GTG = interpolation_matrix.adjoint() * interpolation_matrix;
  GTG.prune([&](const t_uint &i, const t_uint &j, const t_complex &value) {
    return std::abs(value) > 1e-7;
  });
  PURIFY_LOW_LOG("GTG non zeros: {}", GTG.nonZeros());
  return [=](T &output, const T &input) { output = utilities::sparse_multiply_matrix(GTG, input); };
}

template <class T>
sopt::OperatorFunction<T>
base_grid_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                             const Vector<t_real> &w, const Vector<t_complex> &weights,
                             const t_uint &imsizey, const t_uint &imsizex,
                             const t_real &oversample_ratio = 2,
                             const kernels::kernel kernel = kernels::kernel::kb,
                             const t_uint Ju = 4, const t_uint Jv = 4,
                             const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                             const bool w_term = false, const t_uint Jw = 6,
                             const t_real &cellx = 1, const t_real &celly = 1) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(ftkernelu, ftkernelv, imsizey,
                                                              imsizex, oversample_ratio, ft_plan);
  sopt::OperatorFunction<T> GTG;
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::function<t_complex(t_real, t_real, t_real)> kernelw
      = projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                       oversample_ratio);
  GTG = purify::operators::init_gridding_degridding_matrix_2d<T>(u, v, w, weights, imsizey, imsizex,
                                                                 oversample_ratio, kernelv, kernelu,
                                                                 kernelw, Ju, Jv, Jw, w_term);
  return sopt::chained_operators<T>(indirectFZ, GTG, directFZ);
}

template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                             const Vector<t_real> &w, const Vector<t_complex> &weights,
                             const t_uint &imsizey, const t_uint &imsizex,
                             const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                             const t_real &power_tol = 1e-4,
                             const kernels::kernel kernel = kernels::kernel::kb,
                             const t_uint Ju = 4, const t_uint Jv = 4,
                             const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                             const bool w_term = false, const t_uint Jw = 6,
                             const t_real &cellx = 1, const t_real &celly = 1) {

  /*
   *  Returns operator that degrids and grids
   */
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  const sopt::OperatorFunction<T> phiTphi = purify::operators::base_grid_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, Jw,
      cellx, celly);
  auto direct = phiTphi;
  auto id = [](T &out, const T &in) { out = in; };
  const t_real op_norm = details::power_method<T>({direct, N, id, N}, power_iters, power_tol,
                                                  T::Random(imsizex * imsizey));
  const auto operator_norm = purify::operators::init_normalise<T>(op_norm * op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  return direct;
}
template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d(const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                             const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                             const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                             const t_real &power_tol = 1e-4,
                             const kernels::kernel kernel = kernels::kernel::kb,
                             const t_uint Ju = 4, const t_uint Jv = 4,
                             const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                             const bool w_term = false, const t_uint Jw = 6) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_grid_degrid_operator_2d<T>(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                         imsizex, oversample_ratio, power_iters, power_tol, kernel,
                                         Ju, Jv, ft_plan, w_term, Jw, cell_x, cell_y);
}
#ifdef PURIFY_MPI
template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                             const Vector<t_real> &v, const Vector<t_real> &w,
                             const Vector<t_complex> &weights, const t_uint &imsizey,
                             const t_uint &imsizex, const t_real &oversample_ratio = 2,
                             const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                             const kernels::kernel kernel = kernels::kernel::kb,
                             const t_uint Ju = 4, const t_uint Jv = 4,
                             const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                             const bool w_term = false, const t_uint Jw = 6,
                             const t_real &cellx = 1, const t_real &celly = 1) {
  /*
   *  Returns linear transform that is the weighted degridding operator with mpi all sum all
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  const sopt::OperatorFunction<T> phiTphi = purify::operators::base_grid_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, Jw,
      cellx, celly);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = sopt::chained_operators<T>(allsumall, phiTphi);
  auto id = [](T &out, const T &in) { out = in; };
  const t_real op_norm = details::power_method<T>({direct, N, id, N}, power_iters, power_tol,
                                                  comm.broadcast<T>(T::Random(imsizex * imsizey)));
  auto const operator_norm = purify::operators::init_normalise<T>(op_norm * op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  return direct;
}

template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d(const sopt::mpi::Communicator &comm,
                             const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                             const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                             const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                             const t_real &power_tol = 1e-4,
                             const kernels::kernel kernel = kernels::kernel::kb,
                             const t_uint Ju = 4, const t_uint Jv = 4,
                             const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                             const bool w_term = false, const t_uint Jw = 6) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_grid_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights,
                                         imsizey, imsizex, oversample_ratio, power_iters, power_tol,
                                         kernel, Ju, Jv, ft_plan, w_term, Jw, cell_x, cell_y);
}
template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                                 const Vector<t_real> &v, const Vector<t_real> &w,
                                 const Vector<t_complex> &weights, const t_uint &imsizey,
                                 const t_uint &imsizex, const t_real &oversample_ratio = 2,
                                 const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                                 const kernels::kernel kernel = kernels::kernel::kb,
                                 const t_uint Ju = 4, const t_uint Jv = 4,
                                 const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                                 const bool w_term = false, const t_uint Jw = 6,
                                 const t_real &cellx = 1, const t_real &celly = 1) {
  /*
   *  Returns linear transform that is the weighted degridding operator with distributed Fourier
   * grid
   */

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  auto Broadcast = purify::operators::init_broadcaster<T>(comm);
  const sopt::OperatorFunction<T> phiTphi = purify::operators::base_mpi_grid_degrid_operator_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term,
      Jw, cellx, celly);

  auto direct = sopt::chained_operators<T>(Broadcast, phiTphi);
  auto id = [](T &out, const T &in) { out = in; };
  const t_real op_norm = details::power_method<T>({direct, N, id, N}, power_iters, power_tol,
                                                  comm.broadcast<T>(T::Random(imsizex * imsizey)));
  const auto operator_norm = purify::operators::init_normalise<T>(op_norm * op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  return direct;
}

template <class T>
sopt::OperatorFunction<T>
init_grid_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm,
                                 const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                                 const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                                 const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                                 const t_real &power_tol = 1e-4,
                                 const kernels::kernel kernel = kernels::kernel::kb,
                                 const t_uint Ju = 4, const t_uint Jv = 4,
                                 const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                                 const bool w_term = false, const t_uint Jw = 6) {
  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_grid_degrid_operator_2d_mpi<T>(
      comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex, oversample_ratio,
      power_iters, power_tol, kernel, Ju, Jv, ft_plan, w_term, Jw, cell_x, cell_y);
}
#endif
template <class T>
sopt::OperatorFunction<T>
init_psf_convolve_2d(const std::shared_ptr<sopt::LinearTransform<T> const> &degrid_grid,
                     const t_uint &imsizey, const t_uint &imsizex, const t_uint &power_iters = 100,
                     const t_real &power_tol = 1e-4) {
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  t_uint const index = utilities::sub2ind(std::floor(imsizey * 0.5) - 1,
                                          std::floor(imsizex * 0.5) - 1, imsizey, imsizex);
  std::cout << index << std::endl;
  T delta = T::Zero(imsizey * imsizex);
  delta(index) = 1.;
  const T psf = degrid_grid->adjoint() * (*degrid_grid * delta);
  sopt::OperatorFunction<T> fftop, ifftop;
  std::tie(fftop, ifftop)
      = operators::init_FFT_2d<T>(imsizey, imsizex, 1, operators::fftw_plan::estimate);
  T ft_psf = T::Zero(imsizey * imsizex);
  fftop(ft_psf, psf);
  assert(ft_psf.size() == psf.size());
  const auto ft_psf_multiply = [=](T &out, const T &input) {
    out = input;
#pragma omp parallel for
    for(t_uint i = 0; i < input.size(); i++)
      out(i) = ft_psf(i) * input(i);
  };
  const auto psf_convolve = sopt::chained_operators<T>(ifftop, ft_psf_multiply, fftop);
  const auto id = [](T &out, const T &in) { out = in; };
  const t_real op_norm = details::power_method<T>({psf_convolve, N, id, N}, power_iters, power_tol,
                                                  T::Random(imsizex * imsizey));
  const auto operator_norm = purify::operators::init_normalise<T>(op_norm * op_norm);
  return sopt::chained_operators<T>(psf_convolve, operator_norm);
}
template <class T>
sopt::OperatorFunction<T>
init_psf_convolve_2d(const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                     const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                     const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                     const t_real &power_tol = 1e-4,
                     const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                     const t_uint Jv = 4,
                     const operators::fftw_plan ft_plan = operators::fftw_plan::measure) {
  /*
   *  Returns operator conolves image with direction independent point spread function
   */
  const auto meas_op = measurementoperator::init_degrid_operator_2d<T>(
      uv_vis_input, imsizey, imsizex, cell_x, cell_y, oversample_ratio, power_iters, power_tol,
      kernel, Ju, Jv, ft_plan);
  return init_psf_convolve_2d<T>(meas_op, imsizey, imsizex, power_iters, power_tol);
}
} // namespace operators

} // namespace purify
#endif
