#ifndef PURIFY_WPROJ_OPERATORS_H
#define PURIFY_WPROJ_OPERATORS_H

#include "purify/config.h"
#include "purify/types.h"

#include "purify/operators.h"

namespace purify {

namespace details {
//! Given the Fourier transform of a radially symmetric gridding kernel, creates the scaling image
//! for gridding correction.
Image<t_complex> init_correction_radial_2d(const t_real oversample_ratio, const t_uint imsizey_,
                                           const t_uint imsizex_,
                                           const std::function<t_real(t_real)> &ftkerneluv,
                                           const t_real w_mean, const t_real cellx,
                                           const t_real celly);

}  // namespace details

namespace operators {

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_padding_and_FFT_2d(
    const std::function<t_real(t_real)> &ftkerneluv, const t_uint imsizey, const t_uint imsizex,
    const t_real oversample_ratio, const fftw_plan ft_plan, const t_real w_mean, const t_real cellx,
    const t_real celly) {
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  const Image<t_complex> S = purify::details::init_correction_radial_2d(
      oversample_ratio, imsizey, imsizex, ftkerneluv, w_mean, cellx, celly);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG(
      "Constructing Zero Padding "
      "and Correction Operator: "
      "ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  switch (ft_plan) {
  case fftw_plan::measure:
    PURIFY_MEDIUM_LOG("Measuring Plans...");
    break;
  case fftw_plan::estimate:
    PURIFY_MEDIUM_LOG("Estimating Plans...");
    break;
  }
  std::tie(directFFT, indirectFFT) =
      purify::operators::init_FFT_2d<T>(imsizey, imsizex, oversample_ratio, ft_plan);
  auto direct = sopt::chained_operators<T>(directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT);
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const fftw_plan ft_plan, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde) {
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  sopt::OperatorFunction<T> directG, indirectG;
  t_real const w_mean = w_stacking ? w.array().mean() : 0;
  switch (dde) {
  case (dde_type::wkernel_radial): {
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
        std::get<0>(kerneluvs), imsizey, imsizex, oversample_ratio, ft_plan, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    PURIFY_MEDIUM_LOG("Mean, w: {}, +/- {}", w.array().mean(), (w.maxCoeff() - w.minCoeff()) * 0.5);
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
        u, v, w.array() - w_mean, weights, imsizey, imsizex, oversample_ratio,
        std::get<0>(kerneluvs), std::get<1>(kerneluvs), Ju, Jw, cellx, celly, absolute_error,
        relative_error, dde);
    break;
  }
  case (dde_type::wkernel_2d): {
    std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
    std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
        purify::create_kernels(kernel, Ju, Ju, imsizey, imsizex, oversample_ratio);
    std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
        ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    PURIFY_MEDIUM_LOG("Mean, w: {}, +/- {}", w.array().mean(), (w.maxCoeff() - w.minCoeff()) * 0.5);
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
        u, v, w.array() - w_mean, weights, imsizey, imsizex, oversample_ratio,
        std::get<0>(kerneluvs), std::get<1>(kerneluvs), Ju, Jw, cellx, celly, absolute_error,
        relative_error, dde);
    break;
  }
  default:
    throw std::runtime_error("DDE is not recognised.");
  }
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  return std::make_tuple(direct, indirect);
}

#ifdef PURIFY_MPI
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_mpi_all_to_all_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const fftw_plan ft_plan, const bool w_stacking,
    const t_real cellx, const t_real celly, const t_real absolute_error,
    const t_real relative_error, const dde_type dde) {
  const t_uint number_of_images = comm.size();
  if (std::any_of(image_index.begin(), image_index.end(), [&number_of_images](int index) {
        return index < 0 or index > (number_of_images - 1);
      }))
    throw std::runtime_error("Image index is out of bounds");
  const t_int local_grid_size =
      std::floor(imsizex * oversample_ratio) * std::floor(imsizex * oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  sopt::OperatorFunction<T> directG, indirectG;
  t_real const w_mean = w_stacking ? w_stacks.at(comm.rank()) : 0.;
  switch (dde) {
  case (dde_type::wkernel_radial): {
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
        std::get<0>(kerneluvs), imsizey, imsizex, oversample_ratio, ft_plan, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d_all_to_all<T>(
        comm, local_grid_size, comm.rank() * local_grid_size, number_of_images, image_index,
        (w_stacking) ? w_stacks : std::vector<t_real>(comm.size(), 0.), u, v, w, weights, imsizey,
        imsizex, oversample_ratio, std::get<0>(kerneluvs), std::get<1>(kerneluvs), Ju, Jw, cellx,
        celly, absolute_error, relative_error, dde);
    break;
  }
  case (dde_type::wkernel_2d): {
    std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
    std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
        purify::create_kernels(kernel, Ju, Ju, imsizey, imsizex, oversample_ratio);
    std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
        ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, w_mean, cellx, celly);
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
    PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
    PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
    auto const kerneluvs = purify::create_radial_ftkernel(kernel, Ju, oversample_ratio);
    std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d_all_to_all<T>(
        comm, local_grid_size, comm.rank() * local_grid_size, number_of_images, image_index,
        (w_stacking) ? w_stacks : std::vector<t_real>(comm.size(), 0.), u, v, w, weights, imsizey,
        imsizex, oversample_ratio, std::get<0>(kerneluvs), std::get<1>(kerneluvs), Ju, Jw, cellx,
        celly, absolute_error, relative_error, dde);
    break;
  }
  default:
    throw std::runtime_error("DDE is not recognised.");
  }
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  return std::make_tuple(direct, indirect);
}
#endif

}  // namespace operators

namespace measurementoperator {

//! Returns linear transform that is the standard degridding operator
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint imsizey, const t_uint imsizex,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real cellx, const t_real celly, const t_real absolute_error,
    const t_real relative_error, const dde_type dde) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, ft_plan, w_stacking,
      cellx, celly, absolute_error, relative_error, dde);
  auto direct = directDegrid;
  auto indirect = indirectDegrid;
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const utilities::vis_params &uv_vis_input, const t_uint imsizey, const t_uint imsizex,
    const t_real cell_x, const t_real cell_y, const t_real oversample_ratio,
    const kernels::kernel kernel, const t_uint Ju, const t_uint Jw, const bool w_stacking,
    const t_real absolute_error, const t_real relative_error, const dde_type dde) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                    oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x, cell_y,
                                    absolute_error, relative_error, dde);
}
#ifdef PURIFY_MPI
//! Returns linear transform that is the weighted degridding operator with mpi all sum all
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, ft_plan, w_stacking,
      cellx, celly, absolute_error, relative_error, dde);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real absolute_error, const t_real relative_error,
    const dde_type dde) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                    imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x,
                                    cell_y, absolute_error, relative_error, dde);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde) {
  throw std::runtime_error("Under construction!");
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jw, ft_plan, w_stacking,
      cellx, celly, absolute_error, relative_error, dde);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real absolute_error, const t_real relative_error,
    const dde_type dde) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                    imsizex, oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x,
                                    cell_y, absolute_error, relative_error, dde);
}

//! Returns linear transform that is the weighted degridding operator with mpi all to all
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_all_to_all(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio, const kernels::kernel kernel,
    const t_uint Ju, const t_uint Jw, const bool w_stacking, const t_real cellx, const t_real celly,
    const t_real absolute_error, const t_real relative_error, const dde_type dde) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) =
      purify::operators::base_mpi_all_to_all_degrid_operator_2d<T>(
          comm, image_index, w_stacks, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel,
          Ju, Jw, ft_plan, w_stacking, cellx, celly, absolute_error, relative_error, dde);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

//! Returns linear transform that is the weighted degridding operator with mpi all to all
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_all_to_all(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio, const kernels::kernel kernel, const t_uint Ju, const t_uint Jw,
    const bool w_stacking, const t_real absolute_error, const t_real relative_error,
    const dde_type dde) {
  auto uv_vis = uv_vis_input;
  if (uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if (uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d_all_to_all<T>(comm, image_index, w_stacks, uv_vis.u, uv_vis.v,
                                               uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                               oversample_ratio, kernel, Ju, Jw, w_stacking, cell_x,
                                               cell_y, absolute_error, relative_error, dde);
}
#endif
}  // namespace measurementoperator

}  // namespace purify
#endif
