#ifndef PURIFY_OPERATORS_H
#define PURIFY_OPERATORS_H

#include "purify/config.h"
#include <iostream>
#include <tuple>
#include <type_traits>
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include "purify/projection_kernels.h"

#include <fftw3.h>

#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#include "purify/mpi_utilities.h"
#endif

namespace purify {

namespace details {

//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_complex> &weights, const t_uint &imsizey_,
                                          const t_uint &imsizex_, const t_real &oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const std::function<t_real(t_real)> kernelv,
                                          const t_uint Ju = 4, const t_uint Jv = 4);

//! Construct gridding matrix with w projection
Sparse<t_complex>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_real oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv,
                        const std::function<t_complex(t_real, t_real, t_real)> kernelw,
                        const t_uint Ju = 4, const t_uint Jv = 4, const t_uint Jw = 6,
                        const bool w_term = false);

//! Given the Fourier transform of a gridding kernel, creates the scaling image for gridding
//! correction.
Image<t_complex> init_correction2d(const t_real &oversample_ratio, const t_uint &imsizey_,
                                   const t_uint &imsizex_,
                                   const std::function<t_real(t_real)> ftkernelu,
                                   const std::function<t_real(t_real)> ftkernelv,
                                   const t_real &w_mean, const t_real &cellx, const t_real &celly);

template <class T>
t_real power_method(const sopt::LinearTransform<T> &op, const t_uint &niters,
                    const t_real &relative_difference, const T &initial_vector) {

  if(niters <= 0)
    return 1;
  t_real estimate_eigen_value = 1;
  t_real old_value = 0;
  T estimate_eigen_vector = initial_vector;
  estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_vector.matrix().norm();
  PURIFY_DEBUG("Starting power method");
  PURIFY_DEBUG("Iteration: 0, norm = {}", estimate_eigen_value);
  for(t_int i = 0; i < niters; ++i) {
    estimate_eigen_vector = op.adjoint() * (op * estimate_eigen_vector);
    estimate_eigen_value = estimate_eigen_vector.matrix().norm();
    PURIFY_DEBUG("Iteration: {}, norm = {}", i + 1, estimate_eigen_value);
    if(estimate_eigen_value <= 0)
      throw std::runtime_error("Error in operator.");
    if(estimate_eigen_value != estimate_eigen_value)
      throw std::runtime_error("Error in operator or data corrupted.");
    estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_value;
    if(relative_difference * relative_difference
       > std::abs(old_value - estimate_eigen_value) / old_value) {
      old_value = estimate_eigen_value;
      PURIFY_DEBUG("Converged to norm = {}, relative difference < {}", std::sqrt(old_value),
                   relative_difference);
      break;
    }
    old_value = estimate_eigen_value;
  }
  return std::sqrt(old_value);
}

//! Construct gridding matrix with mixing
template <class T, class... ARGS>
Sparse<t_complex> init_gridding_matrix_2d(const Sparse<T> &mixing_matrix, ARGS &&... args) {

  if(mixing_matrix.rows() * mixing_matrix.cols() < 2)
    return init_gridding_matrix_2d(std::forward<ARGS>(args)...);
  const Sparse<t_complex> G = init_gridding_matrix_2d(std::forward<ARGS>(args)...);
  if(mixing_matrix.cols() != G.rows())
    throw std::runtime_error(
        "The columns of the mixing matrix do not match the number of visibilities");
  return mixing_matrix * init_gridding_matrix_2d(std::forward<ARGS>(args)...);
};
} // namespace details

namespace operators {

#ifdef PURIFY_MPI
//! Constructs degridding operator using MPI
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_real oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv,
                        const std::function<t_complex(t_real, t_real, t_real)> kernelw,
                        const t_uint Ju = 4, const t_uint Jv = 4, const t_uint Jw = 6,
                        const bool w_term = false) {

  Sparse<t_complex> interpolation_matrix_original
      = details::init_gridding_matrix_2d(u, v, w, weights, imsizey_, imsizex_, oversample_ratio,
                                         kernelu, kernelv, kernelw, Ju, Jv, Jw, w_term);
  const DistributeSparseVector distributor(interpolation_matrix_original, comm);
  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix
      = std::make_shared<const Sparse<t_complex>>(
          purify::compress_outer(interpolation_matrix_original));
  const std::shared_ptr<const Sparse<t_complex>> adjoint
      = std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

  return std::make_tuple(
      [=](T &output, const T &input) {
        if(comm.is_root()) {
          assert(input.size() > 0);
          distributor.scatter(input, output);
        } else {
          distributor.scatter(output);
        }
        output = utilities::sparse_multiply_matrix(*interpolation_matrix, output);
      },
      [=](T &output, const T &input) {
        if(not comm.is_root()) {
          distributor.gather(utilities::sparse_multiply_matrix(*adjoint, input));
        } else {
          distributor.gather(utilities::sparse_multiply_matrix(*adjoint, input), output);
        }
      });
}

//! Construct MPI broadcast operator
template <class T> sopt::OperatorFunction<T> init_broadcaster(const sopt::mpi::Communicator &comm) {
  return [=](T &output, const T &input) { output = comm.broadcast<T>(input); };
}

//! Construct MPI all sum all operator
template <class T> sopt::OperatorFunction<T> init_all_sum_all(const sopt::mpi::Communicator &comm) {
  return [=](T &output, const T &input) { output = comm.all_sum_all<T>(input); };
}
#endif

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_uint &oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv,
                        const std::function<t_complex(t_real, t_real, t_real)> kernelw,
                        const t_uint Ju = 4, const t_uint Jv = 4, const t_uint Jw = 6,
                        const bool w_term = false) {

  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix
      = std::make_shared<const Sparse<t_complex>>(
          details::init_gridding_matrix_2d(u, v, w, weights, imsizey_, imsizex_, oversample_ratio,
                                           kernelu, kernelv, kernelw, Ju, Jv, Jw, w_term));
  const std::shared_ptr<const Sparse<t_complex>> adjoint
      = std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

  return std::make_tuple(
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(*interpolation_matrix, input);
      },
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(*adjoint, input);
      });
}

//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_zero_padding_2d(const Image<typename T::Scalar> &S, const t_real &oversample_ratio) {
  const t_uint imsizex_ = S.cols();
  const t_uint imsizey_ = S.rows();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
  auto direct = [=](T &output, const T &x) {
    assert(x.size() == imsizex_ * imsizey_);
    output = Vector<t_complex>::Zero(ftsizeu_ * ftsizev_);
#pragma omp parallel for collapse(2)
    for(t_uint j = 0; j < imsizey_; j++) {
      for(t_uint i = 0; i < imsizex_; i++) {
        const t_uint input_index = utilities::sub2ind(j, i, imsizey_, imsizex_);
        const t_uint output_index
            = utilities::sub2ind(y_start + j, x_start + i, ftsizev_, ftsizeu_);
        output(output_index) = S(j, i) * x(input_index);
      }
    }
  };
  auto indirect = [=](T &output, const T &x) {
    assert(x.size() == ftsizeu_ * ftsizev_);
    output = T::Zero(imsizey_ * imsizex_);
#pragma omp parallel for collapse(2)
    for(t_uint j = 0; j < imsizey_; j++) {
      for(t_uint i = 0; i < imsizex_; i++) {
        const t_uint output_index = utilities::sub2ind(j, i, imsizey_, imsizex_);
        const t_uint input_index = utilities::sub2ind(y_start + j, x_start + i, ftsizev_, ftsizeu_);
        output(output_index) = S(j, i) * x(input_index);
      }
    }
  };
  return std::make_tuple(direct, indirect);
}

template <class T> sopt::OperatorFunction<T> init_normalise(const t_real &op_norm) {
  if(not(op_norm > 0))
    throw std::runtime_error("Operator norm is not greater than zero.");
  return [=](T &output, const T &x) { output = x / op_norm; };
}

//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_weights_(const Vector<t_complex> &weights) {
  PURIFY_DEBUG("Calculating weights: W");
  auto direct = [=](T &output, const T &x) {
    assert(weights.size() == x.size());
    output = weights.array() * x.array();
  };
  auto indirect = [=](T &output, const T &x) {
    assert(weights.size() == x.size());
    output = weights.conjugate().array() * x.array();
  };
  return std::make_tuple(direct, indirect);
}
//! enum for fftw plans
enum class fftw_plan { estimate, measure };
//! Construsts FFT operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_FFT_2d(const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_,
            const fftw_plan fftw_plan_flag_ = fftw_plan::measure) {
  t_int const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  t_int const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  t_int plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
  switch(fftw_plan_flag_) {
  case(fftw_plan::measure):
    plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
    break;
  case(fftw_plan::estimate):
    plan_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
    break;
  }

#ifdef PURIFY_OPENMP_FFTW
  PURIFY_LOW_LOG("Using OpenMP threading with FFTW.");
  fftw_init_threads();
  fftw_plan_with_nthreads(omp_get_max_threads());
#endif
  Vector<typename T::Scalar> src = Vector<t_complex>::Zero(ftsizev_ * ftsizeu_);
  Vector<typename T::Scalar> dst = Vector<t_complex>::Zero(ftsizev_ * ftsizeu_);
  // creating plans
  const auto del = [](fftw_plan_s *plan) { fftw_destroy_plan(plan); };
  const std::shared_ptr<fftw_plan_s> m_plan_forward(
      fftw_plan_dft_2d(ftsizev_, ftsizeu_, reinterpret_cast<fftw_complex *>(src.data()),
                       reinterpret_cast<fftw_complex *>(dst.data()), FFTW_FORWARD, plan_flag),
      del);
  const std::shared_ptr<fftw_plan_s> m_plan_inverse(
      fftw_plan_dft_2d(ftsizev_, ftsizeu_, reinterpret_cast<fftw_complex *>(src.data()),
                       reinterpret_cast<fftw_complex *>(dst.data()), FFTW_BACKWARD, plan_flag),
      del);
  auto const direct = [m_plan_forward, ftsizeu_, ftsizev_](T &output, const T &input) {
    assert(input.size() == ftsizev_ * ftsizeu_);
    output = Matrix<typename T::Scalar>::Zero(input.rows(), input.cols());
    fftw_execute_dft(
        m_plan_forward.get(),
        const_cast<fftw_complex *>(reinterpret_cast<const fftw_complex *>(input.data())),
        reinterpret_cast<fftw_complex *>(output.data()));
    output /= std::sqrt(output.size());
  };
  auto const indirect = [m_plan_inverse, ftsizeu_, ftsizev_](T &output, const T &input) {
    assert(input.size() == ftsizev_ * ftsizeu_);
    output = Matrix<typename T::Scalar>::Zero(input.rows(), input.cols());
    fftw_execute_dft(
        m_plan_inverse.get(),
        const_cast<fftw_complex *>(reinterpret_cast<const fftw_complex *>(input.data())),
        reinterpret_cast<fftw_complex *>(output.data()));
    output /= std::sqrt(output.size());
  };
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_padding_and_FFT_2d(const std::function<t_real(t_real)> &ftkernelu,
                        const std::function<t_real(t_real)> &ftkernelv, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const fftw_plan &ft_plan = fftw_plan::measure, const t_real &w_mean = 0,
                        const t_real &cellx = 1, const t_real &celly = 1) {
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;

  const Image<t_complex> S = purify::details::init_correction2d(
      oversample_ratio, imsizey, imsizex, ftkernelu, ftkernelv, w_mean, cellx, celly);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG("Constructing Zero Padding "
                 "and Correction Operator: "
                 "ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  switch(ft_plan) {
  case fftw_plan::measure:
    PURIFY_MEDIUM_LOG("Measuring Plans...");
    break;
  case fftw_plan::estimate:
    PURIFY_MEDIUM_LOG("Estimating Plans...");
    break;
  }
  std::tie(directFFT, indirectFFT)
      = purify::operators::init_FFT_2d<T>(imsizey, imsizex, oversample_ratio, ft_plan);
  auto direct = sopt::chained_operators<T>(directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT);
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4, const fftw_plan &ft_plan = fftw_plan::measure,
                        const bool w_term = false, const t_uint Jw = 6, const t_real &cellx = 1,
                        const t_real &celly = 1) {

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ)
      = base_padding_and_FFT_2d<T>(ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio,
                                   ft_plan, w.array().mean(), cellx, celly);
  sopt::OperatorFunction<T> directG, indirectG;
  if(w_term == true)
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  std::function<t_complex(t_real, t_real, t_real)> kernelw
      = projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                       oversample_ratio);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, w.array() - w.array().mean(), weights, imsizey, imsizex, oversample_ratio, kernelv,
      kernelu, kernelw, Ju, Jv, Jw, w_term);
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  return std::make_tuple(direct, indirect);
}

#ifdef PURIFY_MPI
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_mpi_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const Vector<t_real> &w,
                            const Vector<t_complex> &weights, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real oversample_ratio = 2,
                            const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                            const t_uint Jv = 4,
                            const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                            const bool w_term = false, const t_uint Jw = 6, const t_real &cellx = 1,
                            const t_real &celly = 1) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, 0., cellx, celly);
  sopt::OperatorFunction<T> directG, indirectG;
  if(w_term == true)
    PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                      imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  std::function<t_complex(t_real, t_real, t_real)> kernelw
      = projection_kernels::w_projection_kernel_approx(cellx, celly, imsizex, imsizey,
                                                       oversample_ratio);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, kernelw, Ju, Jv,
      Jw, w_term);
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  if(comm.is_root())
    return std::make_tuple(direct, indirect);
  else
    return std::make_tuple(directG, indirectG);
}
#endif

} // namespace operators

namespace measurementoperator {

//! Returns linear transform that is the standard degridding operator
template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4,
                        const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                        const bool w_term = false, const t_uint Jw = 6, const t_real &cellx = 1,
                        const t_real &celly = 1) {

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, Jw,
      cellx, celly);
  auto direct = directDegrid;
  auto indirect = indirectDegrid;
  const t_real op_norm = details::power_method<T>({direct, M, indirect, N}, power_iters, power_tol,
                                                  T::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<T> const>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d(const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                        const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                        const t_real &power_tol = 1e-4,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4,
                        const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                        const bool w_term = false, const t_uint Jw = 6) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                    oversample_ratio, power_iters, power_tol, kernel, Ju, Jv,
                                    ft_plan, w_term, Jw, cell_x, cell_y);
}

#ifdef PURIFY_MPI
//! Returns linear transform that is the weighted degridding operator with mpi all sum all
template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4,
                        const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                        const bool w_term = false, const t_uint Jw = 6, const t_real &cellx = 1,
                        const t_real &celly = 1) {

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, Jw,
      cellx, celly);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  const t_real op_norm = details::power_method<T>({direct, M, indirect, N}, power_iters, power_tol,
                                                  comm.broadcast<T>(T::Random(imsizex * imsizey)));
  auto const operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<T> const>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d(const sopt::mpi::Communicator &comm,
                        const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                        const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                        const t_real &power_tol = 1e-4,
                        const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                        const t_uint Jv = 4,
                        const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                        const bool w_term = false, const t_uint Jw = 6) {
  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                    imsizex, oversample_ratio, power_iters, power_tol, kernel, Ju,
                                    Jv, ft_plan, w_term, Jw, cell_x, cell_y);
}

//! Returns linear transform that is the weighted degridding operator with a distributed Fourier
//! grid
template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const Vector<t_real> &w,
                            const Vector<t_complex> &weights, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real &oversample_ratio = 2,
                            const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                            const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                            const t_uint Jv = 4,
                            const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                            const bool w_term = false, const t_uint Jw = 6, const t_real &cellx = 1,
                            const t_real &celly = 1) {

  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  auto Broadcast = purify::operators::init_broadcaster<T>(comm);
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_mpi_degrid_operator_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term,
      Jw, cellx, celly);

  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(Broadcast, indirectDegrid);
  const t_real op_norm = details::power_method<T>({direct, M, indirect, N}, power_iters, power_tol,
                                                  comm.broadcast<T>(T::Random(imsizex * imsizey)));
  auto operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return std::make_shared<sopt::LinearTransform<T> const>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T> const>
init_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm,
                            const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                            const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                            const t_real &power_tol = 1e-4,
                            const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4,
                            const t_uint Jv = 4,
                            const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
                            const bool w_term = false, const t_uint Jw = 6) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == utilities::vis_units::lambda)
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == utilities::vis_units::radians)
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));

  return init_degrid_operator_2d_mpi<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                        imsizex, oversample_ratio, power_iters, power_tol, kernel,
                                        Ju, Jv, ft_plan, w_term, Jw, cell_x, cell_y);
}
#endif

template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const>
init_combine_operators(const std::vector<std::shared_ptr<sopt::LinearTransform<T>>> &measure_op,
                       const std::vector<std::tuple<t_uint, t_uint>> seg) {
  const auto direct = [=](T &output, const T &input) {
    for(t_uint i = 0; i < measure_op.size(); i++)
      output.segment(std::get<0>(seg.at(i)), std::get<1>(seg.at(i))) = *(measure_op.at(i)) * input;
  };
  const auto indirect = [=](T &output, const T &input) {
    for(t_uint i = 0; i < measure_op.size(); i++)
      output += measure_op.at(i)->adjoint()
                * input.segment(std::get<0>(seg.at(i)), std::get<1>(seg.at(i)));
  };
  return std::make_shared<sopt::LinearTransform<T> const>(direct, indirect);
}

//! combines different uv_data sets and creates a super measurement operator out of multiple
//! measurement operators
template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const> init_super_operator(
    const std::function<std::shared_ptr<sopt::LinearTransform<T> const>> &create_operator,
    const std::vector<utilities::vis_params> &uv_vis_data, ARGS &&... args) {
  const t_uint blocks = uv_vis_data.size();
  std::vector<std::shared_ptr<sopt::LinearTransform<T>>> measure_ops;
  std::vector<std::tuple<t_uint, t_uint>> seg(blocks);
  t_uint total_measure = 0;
  for(t_int i = 0; i < uv_vis_data.size(); i++) {
    measure_ops.push_back(create_operator(uv_vis_data.at(i), std::forward<ARGS>(args)...));
    seg.emplace_back(total_measure, uv_vis_data.at(i).vis.size());
    total_measure += uv_vis_data.at(i).vis.size();
  }
  if(measure_ops.size() != uv_vis_data.size()) {
    throw std::runtime_error("Numer of groups and measurement operators does not match.");
  }
  return init_combine_operators(measure_ops, seg);
}

template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const> init_super_operator(
    const std::function<std::shared_ptr<sopt::LinearTransform<T> const>> &create_operator,
    const std::vector<std::tuple<t_uint, t_uint>> &seg, const utilities::vis_params &uv_vis_data,
    ARGS &&... args) {
  std::vector<utilities::vis_params> uv_vis_datas(seg.size());
  for(t_uint i = 0; i < seg.size(); i++) {
    uv_vis_datas.emplace_back(uv_vis_data.segment(std::get<0>(seg.at(i)), std::get<1>(seg.at(i))));
  }
  return init_super_operator(create_operator, uv_vis_datas, std::forward<ARGS>(args)...);
}

template <class T, class... ARGS>
std::shared_ptr<sopt::LinearTransform<T> const> init_super_operator(
    const std::function<std::shared_ptr<sopt::LinearTransform<T> const>> &create_operator,
    const utilities::vis_params &uv_vis_data, const t_uint &seg_num, ARGS &&... args) {
  if(seg_num == 0)
    throw std::runtime_error("Number of segments = 0");
  std::vector<std::tuple<t_uint, t_uint>> seg(seg_num);
  const t_uint segment
      = std::floor(static_cast<t_real>(uv_vis_data.size()) / static_cast<t_real>(seg_num));
  t_uint total = 0;
  for(t_uint i = 0; i < seg_num; i++) {
    const t_uint delta = (i == (seg_num - 1)) ? (uv_vis_data.size() - total) : segment;
    seg.emplace_back(total, delta);
    total += delta;
  }
  if(total != uv_vis_data.size())
    throw std::runtime_error("Segments do not sum to the total");
  return init_super_operator(create_operator, uv_vis_data, seg, std::forward<ARGS>(args)...);
}

} // namespace measurementoperator

}; // namespace purify
#endif
