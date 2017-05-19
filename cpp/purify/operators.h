#ifndef PURIFY_OPERATORS_H
#define PURIFY_OPERATORS_H

#include "purify/config.h"
#include <iostream>
#include <tuple>
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/types.h"
#include "purify/utilities.h"
#include "purify/wproj_utilities.h"
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
                                          const t_uint Ju = 4, const t_uint Jv = 4) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  t_uint q;
  t_uint p;
  t_uint index;
  auto omega_to_k = [](const Vector<t_real> &omega) {
    return omega.unaryExpr(std::ptr_fun<double, double>(std::floor));
  };
  const Vector<t_real> k_u = omega_to_k(u - Vector<t_real>::Constant(rows, Ju * 0.5));
  const Vector<t_real> k_v = omega_to_k(v - Vector<t_real>::Constant(rows, Jv * 0.5));

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

#pragma omp simd collapse(3)
  for(t_int m = 0; m < rows; ++m) {
    for(t_int ju = 1; ju <= Ju; ++ju) {
      for(t_int jv = 1; jv <= Jv; ++jv) {
        q = utilities::mod(k_u(m) + ju, ftsizeu_);
        p = utilities::mod(k_v(m) + jv, ftsizev_);
        index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        const t_complex I(0, 1);
        interpolation_matrix.coeffRef(m, index)
            += std::exp(-2 * constant::pi * I * ((k_u(m) + ju) * 0.5 + (k_v(m) + jv) * 0.5))
               * kernelu(u(m) - (k_u(m) + ju)) * kernelv(v(m) - (k_v(m) + jv)) * weights(m);
      }
    }
  }
  return interpolation_matrix;
}
//! Construct gridding matrix with w projection
Sparse<t_complex>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_real oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4, const bool w_term = false, const t_real &cellx = 1,
                        const t_real &celly = 1, const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  const Sparse<t_complex> G = init_gridding_matrix_2d(u, v, weights, imsizey_, imsizex_,
                                                      oversample_ratio, kernelu, kernelv, Ju, Jv);
  if(w_term)
    return wproj_utilities::wprojection_matrix(
        G, std::floor(oversample_ratio * imsizex_), std::floor(oversample_ratio * imsizey_), w,
        cellx, celly, energy_chirp_fraction, energy_kernel_fraction);
  else
    return G;
}

Image<t_real> init_correction2d(const t_real &oversample_ratio, const t_uint &imsizey_,
                                const t_uint imsizex_,
                                const std::function<t_real(t_real)> ftkernelu,
                                const std::function<t_real(t_real)> ftkernelv) {
  /*
     Given the Fourier transform of a gridding kernel, creates the scaling image for gridding
     correction.

*/
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);

  Array<t_real> range;
  range.setLinSpaced(std::max(ftsizeu_, ftsizev_), 0.5, std::max(ftsizeu_, ftsizev_) - 0.5);
  return (1e0 / range.segment(y_start, imsizey_).unaryExpr(ftkernelv)).matrix()
         * (1e0 / range.segment(x_start, imsizex_).unaryExpr(ftkernelu)).matrix().transpose();
}
template <class T>
t_real power_method(const sopt::LinearTransform<T> &op, const t_uint &niters,
                    const t_real &relative_difference, const T &initial_vector) {
  /*
     Attempt at coding the power method, returns thesqrt of the largest eigen value of a linear
     operator composed with its adjoint niters:: max number of iterations relative_difference::
     percentage difference at which eigen value has converged
     */
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
    if(estimate_eigen_value != estimate_eigen_value)
      throw std::runtime_error("Error in measurement operator or data corrupted.");
    estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_value;
    PURIFY_DEBUG("Iteration: {}, norm = {}", i + 1, estimate_eigen_value);
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
} // namespace details
namespace operators {
#ifdef PURIFY_MPI
//! Constructs degridding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_real oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4, const bool w_term = false, const t_real &cellx = 1,
                        const t_real &celly = 1, const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  Sparse<t_complex> interpolation_matrix = details::init_gridding_matrix_2d(
      u, v, w, weights, imsizey_, imsizex_, oversample_ratio, kernelu, kernelv, Ju, Jv, w_term,
      cellx, celly, energy_chirp_fraction, energy_kernel_fraction);
  const DistributeSparseVector distributor(interpolation_matrix, comm);
  interpolation_matrix = purify::compress_outer(interpolation_matrix);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();

  return std::make_tuple(
      [=](T &output, const T &input) {
        if(comm.is_root()) {
          assert(input.size() > 0);
          distributor.scatter(input, output);
        } else {
          distributor.scatter(output);
        }
        output = utilities::sparse_multiply_matrix(interpolation_matrix, output);
      },
      [=](T &output, const T &input) {
        if(not comm.is_root()) {
          distributor.gather(utilities::sparse_multiply_matrix(adjoint, input));
        } else {
          distributor.gather(utilities::sparse_multiply_matrix(adjoint, input), output);
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
//! Constructs degridding operator with mpi
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_uint &oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4, const bool w_term = false, const t_real &cellx = 1,
                        const t_real &celly = 1, const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  const Sparse<t_complex> interpolation_matrix = details::init_gridding_matrix_2d(
      u, v, w, weights, imsizey_, imsizex_, oversample_ratio, kernelu, kernelv, Ju, Jv, w_term,
      cellx, celly, energy_chirp_fraction, energy_kernel_fraction);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();

  return std::make_tuple(
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(interpolation_matrix, input);
      },
      [=](T &output, const T &input) {
        output = utilities::sparse_multiply_matrix(adjoint, input);
      });
}
//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_zero_padding_2d(const Image<t_real> &S, const t_real &oversample_ratio) {
  const t_uint imsizex_ = S.cols();
  const t_uint imsizey_ = S.rows();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
  auto direct = [=](T &output, const T &x) {
    assert(x.size() == imsizex_ * imsizey_);
    output = Vector<t_complex>::Zero(ftsizeu_ * ftsizev_);
#pragma omp simd collapse(2)
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
#pragma omp simd collapse(2)
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

//! Construsts FFT operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_FFT_2d(const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_,
            const std::string &fftw_plan_flag_ = "measure") {
  const std::shared_ptr<FFTOperator> fftop
      = std::make_shared<FFTOperator>();
  auto const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  auto const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  if(fftw_plan_flag_ == "measure") {
    fftop->fftw_flag((FFTW_MEASURE | FFTW_PRESERVE_INPUT));
  } else {
    fftop->fftw_flag((FFTW_ESTIMATE | FFTW_PRESERVE_INPUT));
  }
  fftop->set_up_multithread();

  auto direct = [=](T &output, const T &input) {
    assert(input.size() == ftsizev_ * ftsizeu_);
    Matrix<t_complex> output_image = Image<t_complex>::Map(input.data(), ftsizev_, ftsizeu_);
    output_image = fftop->forward(output_image);
    output = Image<t_complex>::Map(output_image.data(), output_image.size(), 1);
  };
  auto indirect = [=](T &output, const T &input) {
    Matrix<t_complex> output_image = Image<t_complex>::Map(input.data(), ftsizev_, ftsizeu_);
    output_image = fftop->inverse(output_image);
    output = Image<t_complex>::Map(output_image.data(), output_image.size(), 1);
    assert(output.size() == ftsizev_ * ftsizeu_);
  };
  return std::make_tuple(direct, indirect);
}

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const std::string &kernel = "kb", const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure", const bool w_term = false,
                        const t_real &cellx = 1, const t_real &celly = 1,
                        const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG("Constructing Zero Padding and Correction Operator: ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  if(ft_plan == "measure") {
    PURIFY_LOW_LOG("Measuring...");
  } else {
    PURIFY_LOW_LOG("Using an estimate");
  }
  std::tie(directFFT, indirectFFT)
      = purify::operators::init_FFT_2d<T>(imsizey, imsizex, oversample_ratio, ft_plan);
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, Ju, Jv, w_term, cellx,
      celly, energy_chirp_fraction, energy_kernel_fraction);
  auto direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG);
  return std::make_tuple(direct, indirect);
}

#ifdef PURIFY_MPI
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_mpi_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const Vector<t_real> &w,
                            const Vector<t_complex> &weights, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real oversample_ratio = 2,
                            const std::string &kernel = "kb", const t_uint Ju = 4,
                            const t_uint Jv = 4, const std::string &ft_plan = "measure",
                            const bool w_term = false, const t_real &cellx = 1,
                            const t_real &celly = 1, const t_real &energy_chirp_fraction = 1,
                            const t_real &energy_kernel_fraction = 1) {

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG("Constructing Zero Padding and Correction Operators: ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  if(ft_plan == "measure") {
    PURIFY_LOW_LOG("Measuring...");
  } else {
    PURIFY_LOW_LOG("Using an estimate");
  }
  std::tie(directFFT, indirectFFT)
      = purify::operators::init_FFT_2d<T>(imsizey, imsizex, oversample_ratio, ft_plan);
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Kernel Support: {} x {}", Ju, Jv);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, Ju, Jv, w_term,
      cellx, celly, energy_chirp_fraction, energy_kernel_fraction);
  auto direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG);
  if(comm.is_root())
    return std::make_tuple(direct, indirect);
  else
    return std::make_tuple(directG, indirectG);
}
#endif
}

namespace measurementoperator {

template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                        const std::string &kernel = "kb", const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure", const bool w_term = false,
                        const t_real &cellx = 1, const t_real &celly = 1,
                        const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  /*
   *  Returns linear transform that is the standard degridding operator
   */
  const t_int M = u.size();
  const t_int N = imsizey * imsizex;
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, cellx,
      celly, energy_chirp_fraction, energy_kernel_fraction);
  auto direct = directDegrid;
  auto indirect = indirectDegrid;
  const t_real op_norm
      = details::power_method<T>({direct, {0, 1, M}, indirect, {0, 1, N}}, power_iters, power_tol,
                                 T::Random(imsizex * imsizey));
  auto operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return {direct, {0, 1, M}, indirect, {0, 1, N}};
}
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                        const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                        const t_real &power_tol = 1e-4, const std::string &kernel = "kb",
                        const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure", const bool w_term = false,
                        const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == "lambda")
    uv_vis = utilities::set_cell_size(uv_vis, cell_x, cell_y);
  if(uv_vis.units == "radians")
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                    oversample_ratio, power_iters, power_tol, kernel, Ju, Jv,
                                    ft_plan, w_term, cell_x, cell_y, energy_chirp_fraction,
                                    energy_kernel_fraction);
}
#ifdef PURIFY_MPI
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &oversample_ratio = 2,
                        const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                        const std::string &kernel = "kb", const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure", const bool w_term = false,
                        const t_real &cellx = 1, const t_real &celly = 1,
                        const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {
  /*
   *  Returns linear transform that is the weighted degridding operator with mpi all sum all
   */

  const t_int M = u.size();
  const t_int N = imsizey * imsizex;
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term, cellx,
      celly, energy_chirp_fraction, energy_kernel_fraction);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  const t_real op_norm
      = details::power_method<T>({direct, {0, 1, M}, indirect, {0, 1, N}}, power_iters, power_tol,
                                 comm.broadcast<T>(T::Random(imsizex * imsizey)));
  auto const operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return {direct, {0, 1, M}, indirect, {0, 1, N}};
} // namespace measurementoperator
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const sopt::mpi::Communicator &comm,
                        const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                        const t_real &oversample_ratio = 2, const t_uint &power_iters = 100,
                        const t_real &power_tol = 1e-4, const std::string &kernel = "kb",
                        const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure", const bool w_term = false,
                        const t_real &energy_chirp_fraction = 1,
                        const t_real &energy_kernel_fraction = 1) {

  auto uv_vis = uv_vis_input;
  if(uv_vis.units == "lambda")
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == "radians")
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                    imsizex, oversample_ratio, power_iters, power_tol, kernel, Ju,
                                    Jv, ft_plan, w_term, cell_x, cell_y, energy_chirp_fraction,
                                    energy_kernel_fraction);

} // namespace measurementoperator
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const Vector<t_real> &w,
                            const Vector<t_complex> &weights, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real &oversample_ratio = 2,
                            const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                            const std::string &kernel = "kb", const t_uint Ju = 4,
                            const t_uint Jv = 4, const std::string &ft_plan = "measure",
                            const bool w_term = false, const t_real &cellx = 1,
                            const t_real &celly = 1, const t_real &energy_chirp_fraction = 1,
                            const t_real &energy_kernel_fraction = 1) {
  /*
   *  Returns linear transform that is the weighted degridding operator with a distributed Fourier
   * grid
   */

  const t_int M = u.size();
  const t_int N = imsizey * imsizex;
  auto Broadcast = purify::operators::init_broadcaster<T>(comm);
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_mpi_degrid_operator_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_term,
      cellx, celly, energy_chirp_fraction, energy_kernel_fraction);

  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(Broadcast, indirectDegrid);
  const t_real op_norm
      = details::power_method<T>({direct, {0, 1, M}, indirect, {0, 1, N}}, power_iters, power_tol,
                                 comm.broadcast<T>(T::Random(imsizex * imsizey)));
  auto operator_norm = purify::operators::init_normalise<T>(op_norm);
  direct = sopt::chained_operators<T>(direct, operator_norm);
  indirect = sopt::chained_operators<T>(operator_norm, indirect);
  return {direct, {0, 1, M}, indirect, {0, 1, N}};
} // namespace purify
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm,
                            const utilities::vis_params &uv_vis_input, const t_uint &imsizey,
                            const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
                            const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                            const t_real &power_tol = 1e-4, const std::string &kernel = "kb",
                            const t_uint Ju = 4, const t_uint Jv = 4,
                            const std::string &ft_plan = "measure", const bool w_term = false,
                            const t_real &energy_chirp_fraction = 1,
                            const t_real &energy_kernel_fraction = 1) {
  auto uv_vis = uv_vis_input;
  if(uv_vis.units == "lambda")
    uv_vis = utilities::set_cell_size(comm, uv_vis, cell_x, cell_y);
  if(uv_vis.units == "radians")
    uv_vis = utilities::uv_scale(uv_vis, std::floor(oversample_ratio * imsizex),
                                 std::floor(oversample_ratio * imsizey));
  return init_degrid_operator_2d_mpi<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                        imsizex, oversample_ratio, power_iters, power_tol, kernel,
                                        Ju, Jv, ft_plan, w_term, cell_x, cell_y,
                                        energy_chirp_fraction, energy_kernel_fraction);
}
#endif
} // namespace measurementoperator
}; // namespace purify
#endif
