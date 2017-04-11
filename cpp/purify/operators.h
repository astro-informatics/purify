#ifndef PURIFY_OPERATORS_H
#define PURIFY_OPERATORS_H

#include "purify/config.h"
#include <tuple>
#include "sopt/chained_operators.h"
#include "sopt/linear_transform.h"
#include "purify/kernels.h"
#include "purify/types.h"
#include "purify/utilities.h"
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#endif
namespace purify {

namespace details {
//! Construct gridding matrix
Sparse<t_complex>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_uint oversample_ratio,
                        const t_uint resample_factor, const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4) {
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_LOW_LOG("Constructing Gridding Operator: G");
  const t_uint ftsizev_ = std::floor(std::floor(imsizey_ * oversample_ratio) * resample_factor);
  const t_uint ftsizeu_ = std::floor(std::floor(imsizex_ * oversample_ratio) * resample_factor);
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
               * kernelu(u(m) - (k_u(m) + ju)) * kernelv(v(m) - (k_v(m) + jv));
      }
    }
  }
  return interpolation_matrix;
}
//! Construsts gridding correction operator
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
  if(niters == 0)
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
    estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_value;
    PURIFY_DEBUG("Iteration: {}, norm = {}", i + 1, estimate_eigen_value);
    if(relative_difference * relative_difference
       > std::abs(old_value - estimate_eigen_value) / old_value) {
      old_value = estimate_eigen_value;
      break;
    }
    old_value = estimate_eigen_value;
  }
  PURIFY_DEBUG("Converged to norm = {}, relative difference < {}", std::sqrt(old_value),
               relative_difference);
  return std::sqrt(old_value);
}
} // namespace details
namespace operators {
#ifdef PURIFY_MPI
//! Constructs degridding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const sopt::mpi::Communicator comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const t_uint &imsizey_, const t_uint &imsizex_,
                        const t_uint oversample_ratio, const t_uint resample_factor,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4) {

  Sparse<t_complex> interpolation_matrix = details::init_gridding_matrix_2d(
      u, v, imsizey_, imsizex_, oversample_ratio, resample_factor, kernelu, kernelv, Ju, Jv);
  const DistributeSparseVector distributor(interpolation_matrix, comm);
  interpolation_matrix = purify::compress_outer(interpolation_matrix);
  const Sparse<t_complex> adjoint = interpolation_matrix.adjoint();

  return std::make_tuple(
      [=](T &output, const T &input) {
        if(comm.is_root()) {
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
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_broadcaster(const sopt::mpi::Communicator comm) {
  return std::make_tuple([=](T &output, const T &input) { output = input; },
                         [=](T &output, const T &input) { output = comm.broadcast(input); });
}
//! Construct MPI all sum all operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_all_sum_all(const sopt::mpi::Communicator comm) {
  return std::make_tuple([=](T &output, const T &input) { output = input; },
                         [=](T &output, const T &input) { output = comm.all_sum_all(input); });
}
#endif
//! Constructs degridding operator with mpi
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_uint oversample_ratio,
                        const t_uint resample_factor, const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju = 4,
                        const t_uint Jv = 4) {

  const Sparse<t_complex> interpolation_matrix = details::init_gridding_matrix_2d(
      u, v, imsizey_, imsizex_, oversample_ratio, resample_factor, kernelu, kernelv, Ju, Jv);
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
  PURIFY_LOW_LOG("Constructing Zero Padding Operator: Z");
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  const t_uint imsizex_ = S.cols();
  const t_uint imsizey_ = S.rows();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
  auto direct = [=](T &output, const T &x) {
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
    output = Vector<t_complex>::Zero(imsizey_ * imsizex_);
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

//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_weights_(const Vector<t_complex> &weights, const std::string &weighting_type_,
              const t_real &R_, const t_uint &imsizex_, const t_uint &imsizey_,
              const t_real &oversample_ratio, const Vector<t_real> &u, const Vector<t_real> &v) {
  PURIFY_DEBUG("Calculating weights: W");
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const Vector<t_complex> W = utilities::init_weights(u, v, weights, oversample_ratio,
                                                      weighting_type_, R_, ftsizeu_, ftsizev_);
  auto direct = [=](T &output, const T &x) { output = W.array() * x.array(); };
  auto indirect = [=](T &output, const T &x) { output = W.conjugate().array() * x.array(); };
  return std::make_tuple(direct, indirect);
}

//! Construsts FFT operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
init_FFT_2d(const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_,
            const t_real &resample_factor, const std::string &fftw_plan_flag_ = "measure") {
  const std::shared_ptr<FFTOperator> fftop
      = std::make_shared<FFTOperator>(FFTOperator::FFTOperator());
  auto const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  auto const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  PURIFY_LOW_LOG("Planning FFT operator");
  if(fftw_plan_flag_ == "measure") {
    PURIFY_LOW_LOG("Measuring...");
    fftop->fftw_flag((FFTW_MEASURE | FFTW_PRESERVE_INPUT));
  } else {
    PURIFY_LOW_LOG("Using an estimate");
    fftop->fftw_flag((FFTW_ESTIMATE | FFTW_PRESERVE_INPUT));
  }
  fftop->set_up_multithread();

  auto direct = [=](T &output, const T &input) {
    Matrix<t_complex> output_image = Image<t_complex>::Map(input.data(), ftsizev_, ftsizeu_);
    output_image = utilities::re_sample_ft_grid(fftop->forward(output_image), resample_factor);
    output = Image<t_complex>::Map(output_image.data(), output_image.size(), 1);
  };
  auto indirect = [=](T &output, const T &input) {
    Matrix<t_complex> output_image
        = Image<t_complex>::Map(input.data(), std::floor(ftsizev_ * resample_factor),
                                std::floor(ftsizeu_ * resample_factor));
    output_image = fftop->inverse(utilities::re_sample_ft_grid(output_image, 1. / resample_factor));
    output = Image<t_complex>::Map(output_image.data(), output_image.size(), 1);
  };
  return std::make_tuple(direct, indirect);
}
}

namespace measurementoperator {
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v, const t_uint &imsizey,
                        const t_uint &imsizex, const t_real oversample_ratio = 2,
                        const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                        const t_real resample_factor = 1., const std::string &kernel = "kb",
                        const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure") {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  auto direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG);
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm
      = details::power_method<T>(measure_op, power_iters, power_tol, T::Random(imsizex * imsizey));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG);
  return {direct, indirect};
}
template <class T>
sopt::LinearTransform<T>
init_degrid_weighted_operator_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                 const Vector<t_complex> &weights, const t_uint &imsizey,
                                 const t_uint &imsizex, const t_real oversample_ratio = 2,
                                 const t_uint &power_iters = 100, const t_real &power_tol = 1e-4,
                                 const t_real resample_factor = 1.,
                                 const std::string &kernel = "kb", const t_uint Ju = 4,
                                 const t_uint Jv = 4, const std::string &ft_plan = "measure",
                                 const std::string &weighting_type = "none", const t_real &R = 0) {
  /*
   *  Returns linear transform that is the standard degridding operator
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  std::tie(directW, indirectW) = purify::operators::init_weights_<T>(
      weights, weighting_type, R, imsizex, imsizey, oversample_ratio, u, v);
  auto direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG, indirectW);
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm
      = details::power_method<T>(measure_op, power_iters, power_tol, T::Random(imsizex * imsizey));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
  indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG, indirectW);
  return {direct, indirect};
}
#ifdef PURIFY_MPI
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                        const Vector<t_real> &v, const t_uint &imsizey, const t_uint &imsizex,
                        const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                        const t_real &power_tol = 1e-4, const t_real resample_factor = 1.,
                        const std::string &kernel = "kb", const t_uint Ju = 4, const t_uint Jv = 4,
                        const std::string &ft_plan = "measure") {
  /*
   *  Returns linear transform that is the standard degridding operator with mpi all sum all
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directAllsumall, indirectAllsumall;
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directAllsumall, indirectAllsumall) = purify::operators::init_all_sum_all<T>(comm);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  auto direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG);
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm
      = details::power_method<T>(measure_op, power_iters, power_tol, T::Random(imsizex * imsizey));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  direct = sopt::chained_operators<T>(directG, directFFT, directZ);
  indirect = sopt::chained_operators<T>(indirectAllsumall, indirectZ, indirectFFT, indirectG);
  return {direct, indirect};
}
template <class T>
sopt::LinearTransform<T>
init_degrid_weighted_operator_2d(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                                 const Vector<t_real> &v, const Vector<t_complex> &weights,
                                 const t_uint &imsizey, const t_uint &imsizex,
                                 const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                                 const t_real &power_tol = 1e-4, const t_real resample_factor = 1.,
                                 const std::string &kernel = "kb", const t_uint Ju = 4,
                                 const t_uint Jv = 4, const std::string &ft_plan = "measure",
                                 const std::string &weighting_type = "none", const t_real &R = 0) {
  /*
   *  Returns linear transform that is the weighted degridding operator with mpi all sum all
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directAllsumall, indirectAllsumall;
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directAllsumall, indirectAllsumall) = purify::operators::init_all_sum_all<T>(comm);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  std::tie(directW, indirectW) = purify::operators::init_weights_<T>(
      weights, weighting_type, R, imsizex, imsizey, oversample_ratio, u, v);
  auto direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT, indirectG, indirectW);
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm
      = details::power_method<T>(measure_op, power_iters, power_tol, T::Random(imsizex * imsizey));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
  indirect
      = sopt::chained_operators<T>(indirectAllsumall, indirectZ, indirectFFT, indirectG, indirectW);
  return {direct, indirect};
}
template <class T>
sopt::LinearTransform<T>
init_degrid_operator_2d_mpi(const sopt::mpi::Communicator &comm, const Vector<t_real> &u,
                            const Vector<t_real> &v, const t_uint &imsizey, const t_uint &imsizex,
                            const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
                            const t_real &power_tol = 1e-4, const t_real resample_factor = 1.,
                            const std::string &kernel = "kb", const t_uint Ju = 4,
                            const t_uint Jv = 4, const std::string &ft_plan = "measure") {
  /*
   *  Returns linear transform that is the standard degridding operator with distributed Fourier
   * grid
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directBroadcast, indirectBroadcast;
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directBroadcast, indirectBroadcast) = purify::operators::init_broadcaster<T>(comm);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      comm, u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  sopt::OperatorFunction<T> direct;
  sopt::OperatorFunction<T> indirect;
  if(comm.is_root()) {
    direct = sopt::chained_operators<T>(directG, directFFT, directZ);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectZ, indirectFFT, indirectG);
  } else {
    direct = sopt::chained_operators<T>(directG);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectG);
  }
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm = details::power_method<T>(measure_op, power_iters, power_tol,
                                                  comm.broadcast<T>(T::Random(imsizex * imsizey)));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  if(comm.is_root()) {
    direct = sopt::chained_operators<T>(directG, directFFT, directZ);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectZ, indirectFFT, indirectG);
  } else {
    direct = sopt::chained_operators<T>(directG);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectG);
  }
  return {direct, indirect};
}
template <class T>
sopt::LinearTransform<T> init_degrid_weighted_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real oversample_ratio = 2, const t_uint &power_iters = 100,
    const t_real &power_tol = 1e-4, const t_real resample_factor = 1.,
    const std::string &kernel = "kb", const t_uint Ju = 4, const t_uint Jv = 4,
    const std::string &ft_plan = "measure", const std::string &weighting_type = "none",
    const t_real &R = 0) {
  /*
   *  Returns linear transform that is the weighted degridding operator with a distributed Fourier
   * grid
   */

  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv)
      = purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directBroadcast, indirectBroadcast;
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  sopt::OperatorFunction<T> directG, indirectG;
  sopt::OperatorFunction<T> directW, indirectW;
  const Image<t_real> S = purify::details::init_correction2d(oversample_ratio, imsizey, imsizex,
                                                             ftkernelu, ftkernelv);
  std::tie(directBroadcast, indirectBroadcast) = purify::operators::init_broadcaster<T>(comm);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey, imsizex, oversample_ratio, resample_factor, ft_plan);
  std::tie(directG, indirectG) = purify::operators::init_gridding_matrix_2d<T>(
      comm, u, v, imsizey, imsizex, oversample_ratio, resample_factor, kernelv, kernelu, Ju, Jv);
  std::tie(directW, indirectW) = purify::operators::init_weights_<Vector<t_complex>>(
      weights, weighting_type, R, imsizex, imsizey, oversample_ratio, u, v);
  sopt::OperatorFunction<T> direct;
  sopt::OperatorFunction<T> indirect;
  if(comm.is_root()) {
    direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectZ, indirectFFT, indirectG,
                                          indirectW);
  } else {
    direct = sopt::chained_operators<T>(directW, directG);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectG, indirectW);
  }
  const sopt::LinearTransform<T> measure_op = {direct, indirect};
  const t_real op_norm = comm.broadcast<t_real>(details::power_method<T>(
      measure_op, power_iters, power_tol, comm.broadcast<T>(T::Random(imsizex * imsizey))));
  std::tie(directZ, indirectZ)
      = purify::operators::init_zero_padding_2d<T>(S / op_norm, oversample_ratio);
  if(comm.is_root()) {
    direct = sopt::chained_operators<T>(directW, directG, directFFT, directZ);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectZ, indirectFFT, indirectG,
                                          indirectW);
  } else {
    direct = sopt::chained_operators<T>(directW, directG);
    indirect = sopt::chained_operators<T>(indirectBroadcast, indirectG, indirectW);
  }
  return {direct, indirect};
}
#endif
} // namespace measurementoperator
}; // namespace purify
#endif
