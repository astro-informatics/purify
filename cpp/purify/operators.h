#ifndef PURIFY_OPERATORS_H
#define PURIFY_OPERATORS_H

#include "purify/config.h"
#include "purify/types.h"
#include <iostream>
#include <tuple>
#include <type_traits>
#include "purify/kernels.h"
#include "purify/logging.h"
#include "purify/utilities.h"
#include "purify/uvw_utilities.h"
#include "purify/wide_field_utilities.h"
#include "purify/wkernel_integration.h"
#include <sopt/chained_operators.h>
#include <sopt/linear_transform.h>
#include "purify/fly_operators.h"

#include <fftw3.h>

#ifdef PURIFY_MPI
#include "purify/AllToAllSparseVector.h"
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#include "purify/mpi_utilities.h"
#include <sopt/mpi/communicator.h>
#endif

#include "purify/fly_operators.h"

namespace purify {

namespace details {

//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_complex> &weights, const t_uint &imsizey_,
                                          const t_uint &imsizex_, const t_real &oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const std::function<t_real(t_real)> kernelv,
                                          const t_uint Ju = 4, const t_uint Jv = 4);
//! Construct gridding matrix with wprojection
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_real> &w, const Vector<t_complex> &weights,
                                          const t_uint imsizey_, const t_uint imsizex_,
                                          const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> &ftkerneluv,
                                          const std::function<t_real(t_real)> &kerneluv,
                                          const t_uint Ju, const t_uint Jw, const t_real cellx,
                                          const t_real celly, const t_real abs_error,
                                          const t_real rel_error, const dde_type dde);

//! Construct all to all gridding matrix
template <class STORAGE_INDEX_TYPE = t_int>
Sparse<t_complex, STORAGE_INDEX_TYPE> init_gridding_matrix_2d(
    const t_uint number_of_images, const std::vector<t_int> &image_index, const Vector<t_real> &u,
    const Vector<t_real> &v, const Vector<t_complex> &weights, const t_uint &imsizey_,
    const t_uint &imsizex_, const t_real &oversample_ratio,
    const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv,
    const t_uint Ju, const t_uint Jv) {
  if (std::any_of(image_index.begin(), image_index.end(), [&number_of_images](int index) {
        return index < 0 or index > (number_of_images - 1);
      }))
    throw std::runtime_error("Image index is out of bounds");
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const STORAGE_INDEX_TYPE cols = ftsizeu_ * ftsizev_ * number_of_images;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  Sparse<t_complex, STORAGE_INDEX_TYPE> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  const t_int jv_max = std::min(Jv, ftsizev_);
  // If I collapse this for loop there is a crash when using MPI... Sparse<>::insert() doesn't work
  // right
#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    for (t_int ju = 1; ju < ju_max + 1; ++ju) {
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_real k_u = std::floor(u(m) - ju_max * 0.5);
        const t_real k_v = std::floor(v(m) - jv_max * 0.5);
        const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        assert(image_index.at(m) < number_of_images);
        const STORAGE_INDEX_TYPE index =
            static_cast<STORAGE_INDEX_TYPE>(utilities::sub2ind(p, q, ftsizev_, ftsizeu_)) +
            static_cast<STORAGE_INDEX_TYPE>(image_index.at(m)) *
                static_cast<STORAGE_INDEX_TYPE>(ftsizev_ * ftsizeu_);
        interpolation_matrix.insert(static_cast<STORAGE_INDEX_TYPE>(m), index) =
            std::exp(-2 * constant::pi * I * ((k_u + ju) * 0.5 + (k_v + jv) * 0.5)) *
            kernelu(u(m) - (k_u + ju)) * kernelv(v(m) - (k_v + jv)) * weights(m);
      }
    }
  }
  return interpolation_matrix;
}
//! Construct all to all gridding matrix with wprojection
template <class STORAGE_INDEX_TYPE = t_int>
Sparse<t_complex, STORAGE_INDEX_TYPE> init_gridding_matrix_2d(
    const t_uint number_of_images, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey_,
    const t_uint imsizex_, const t_real oversample_ratio,
    const std::function<t_real(t_real)> &ftkerneluv, const std::function<t_real(t_real)> &kerneluv,
    const t_uint Ju, const t_uint Jw, const t_real du, const t_real dv,
    const t_real abs_error, const t_real rel_error, const dde_type dde) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  PURIFY_HIGH_LOG("du x du: {}, {}", du, dv);
  if (std::abs(du - dv) > 1e-6)
    throw std::runtime_error(
        "Field of view along l and m is not the same, this assumption is required for "
        "w-projection.");
  const t_uint rows = u.size();
  const STORAGE_INDEX_TYPE cols = ftsizeu_ * ftsizev_ * number_of_images;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");
  if (u.size() != w.size())
    throw std::runtime_error(
        "Size of u and w vectors are not the same for creating gridding matrix.");
  if (u.size() != weights.size())
    throw std::runtime_error(
        "Size of u and w vectors are not the same for creating gridding matrix.");
  if (Ju > Jw)
    throw std::runtime_error(
        "w kernel size must be at least the size of w=0 kernel, must have Ju <= Jw.");
  // count gridding coefficients with variable support size
  Vector<t_int> total_coeffs = Vector<t_int>::Zero(w.size());
  for (int i = 0; i < w.size(); i++) {
    const t_int Ju_max = widefield::w_support(w(i) - w_stacks.at(image_index.at(i)), du,
                                              static_cast<t_int>(Ju), static_cast<t_int>(Jw));
    total_coeffs(i) = Ju_max * Ju_max;
  }
  const t_real num_of_coeffs = total_coeffs.array().cast<t_real>().sum();
  PURIFY_HIGH_LOG("Using {} rows (coefficients per a row {}), and memory of {} MegaBytes",
                  total_coeffs.size(), total_coeffs.array().cast<t_real>().mean(),
                  16. * num_of_coeffs / std::pow(10., 6));
  Sparse<t_complex, STORAGE_INDEX_TYPE> interpolation_matrix(static_cast<STORAGE_INDEX_TYPE>(rows),
                                                             cols);
  try {
    interpolation_matrix.reserve(total_coeffs);
  } catch (std::bad_alloc e) {
    throw std::runtime_error(
        "Not enough memory for coefficients, choose upper limit on support size Jw.");
  }

  const t_complex I(0., 1.);

  const t_uint max_evaluations = 1e8;
  const t_real relative_error = rel_error;
  const t_real absolute_error = abs_error;

  auto const ftkernel_radial = [&](const t_real l) -> t_real { return ftkerneluv(l); };

  std::int64_t coeffs_done = 0;
  std::int64_t total = 0;

#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    // w_projection convolution setup
    const t_real w_val = w(m) - w_stacks.at(image_index.at(m));
    const t_int Ju_max = widefield::w_support(w_val, du, Ju, Jw);
    t_uint evaluations = 0;
    const t_int kwu = std::floor(u(m) - Ju_max * 0.5);
    const t_int kwv = std::floor(v(m) - Ju_max * 0.5);

    for (t_int ju = 1; ju < Ju_max + 1; ++ju) {
      for (t_int jv = 1; jv < Ju_max + 1; ++jv) {
        const t_uint q = utilities::mod(kwu + ju, ftsizeu_);
        const t_uint p = utilities::mod(kwv + jv, ftsizev_);
        const STORAGE_INDEX_TYPE index =
            static_cast<STORAGE_INDEX_TYPE>(utilities::sub2ind(p, q, ftsizev_, ftsizeu_)) +
            static_cast<STORAGE_INDEX_TYPE>(image_index.at(m)) *
                static_cast<STORAGE_INDEX_TYPE>(ftsizev_ * ftsizeu_);
        interpolation_matrix.insert(static_cast<STORAGE_INDEX_TYPE>(m), index) =
            std::exp(-2 * constant::pi * I * ((kwu + ju) * 0.5 + (kwv + jv) * 0.5)) * weights(m) *
            ((dde == dde_type::wkernel_radial)
                 ? projection_kernels::exact_w_projection_integration_1d(
                       (u(m) - (kwu + ju)), (v(m) - (kwv + jv)), w_val, du, oversample_ratio,
                       ftkernel_radial, max_evaluations, absolute_error, relative_error,
                       (du > 1.) ? integration::method::p : integration::method::h, evaluations)
                 : projection_kernels::exact_w_projection_integration(
                       (u(m) - (kwu + ju)), (v(m) - (kwv + jv)), w_val, du, dv, oversample_ratio,
                       ftkernel_radial, ftkernel_radial, max_evaluations, absolute_error,
                       relative_error, integration::method::h, evaluations));
#pragma omp critical(add_eval)
        {
          coeffs_done++;
          if (num_of_coeffs > 100) {
            if ((coeffs_done % (static_cast<t_int>(num_of_coeffs / 100)) == 0)) {
              PURIFY_LOW_LOG(
                  "w = {}, support = {}x{}, coeffs: {} of {}, {}%", w_val, Ju_max, Ju_max,
                  coeffs_done, num_of_coeffs,
                  static_cast<t_real>(coeffs_done) / static_cast<t_real>(num_of_coeffs) * 100.);
            }
          }
        }
      }
    }
  }
  interpolation_matrix.makeCompressed();
  return interpolation_matrix;
}

//! Given the Fourier transform of a gridding kernel, creates the scaling image for gridding
//! correction.
Image<t_complex> init_correction2d(const t_real oversample_ratio, const t_uint imsizey_,
                                   const t_uint imsizex_,
                                   const std::function<t_real(t_real)> &ftkernelu,
                                   const std::function<t_real(t_real)> &ftkernelv,
                                   const t_real w_mean, const t_real dl, const t_real dm);

//! Construct gridding matrix with mixing
template <class T, class... ARGS>
Sparse<t_complex> init_gridding_matrix_2d(const Sparse<T> &mixing_matrix, ARGS &&... args) {
  if (mixing_matrix.rows() * mixing_matrix.cols() < 2)
    return init_gridding_matrix_2d(std::forward<ARGS>(args)...);
  const Sparse<t_complex> G = init_gridding_matrix_2d(std::forward<ARGS>(args)...);
  if (mixing_matrix.cols() != G.rows())
    throw std::runtime_error(
        "The columns of the mixing matrix do not match the number of visibilities");
  return mixing_matrix * init_gridding_matrix_2d(std::forward<ARGS>(args)...);
};
}  // namespace details

namespace operators {

#ifdef PURIFY_MPI
//! Constructs degridding operator using MPI
template <class T, class... ARGS>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_gridding_matrix_2d(
    const sopt::mpi::Communicator &comm, ARGS &&... args) {
  Sparse<t_complex> interpolation_matrix_original =
      details::init_gridding_matrix_2d(std::forward<ARGS>(args)...);
  const DistributeSparseVector distributor(interpolation_matrix_original, comm);
  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix =
      std::make_shared<const Sparse<t_complex>>(
          purify::compress_outer(interpolation_matrix_original));
  const std::shared_ptr<const Sparse<t_complex>> adjoint =
      std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

  return std::make_tuple(
      [=](T &output, const T &input) {
        if (comm.is_root()) {
          assert(input.size() > 0);
          distributor.scatter(input, output);
        } else {
          distributor.scatter(output);
        }
        output = utilities::sparse_multiply_matrix(*interpolation_matrix, output);
      },
      [=](T &output, const T &input) {
        if (not comm.is_root()) {
          distributor.gather(utilities::sparse_multiply_matrix(*adjoint, input));
        } else {
          distributor.gather(utilities::sparse_multiply_matrix(*adjoint, input), output);
        }
      });
}

//! Constructs degridding operator using MPI all to all
template <class T, class STORAGE_INDEX_TYPE, class... ARGS>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_gridding_matrix_2d_all_to_all(
    const sopt::mpi::Communicator &comm, const STORAGE_INDEX_TYPE local_grid_size,
    const STORAGE_INDEX_TYPE start_index, const t_uint number_of_images,
    const std::vector<t_int> &image_index, ARGS &&... args) {
  Sparse<t_complex, STORAGE_INDEX_TYPE> interpolation_matrix_original =
      details::init_gridding_matrix_2d<STORAGE_INDEX_TYPE>(number_of_images, image_index,
                                                           std::forward<ARGS>(args)...);
  const AllToAllSparseVector<STORAGE_INDEX_TYPE> distributor(interpolation_matrix_original,
                                                             local_grid_size, start_index, comm);
  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix =
      std::make_shared<const Sparse<t_complex>>(
          purify::compress_outer(interpolation_matrix_original));
  const std::shared_ptr<const Sparse<t_complex>> adjoint =
      std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

  return std::make_tuple(
      [=](T &output, const T &input) {
        assert(input.size() > 0);
        distributor.recv_grid(input, output);
        output = utilities::sparse_multiply_matrix(*interpolation_matrix, output);
      },
      [=](T &output, const T &input) {
        distributor.send_grid(utilities::sparse_multiply_matrix(*adjoint, input), output);
      });
}

//! Construct MPI broadcast operator
template <class T>
sopt::OperatorFunction<T> init_broadcaster(const sopt::mpi::Communicator &comm) {
  return [=](T &output, const T &input) { output = comm.broadcast<T>(input); };
}

//! Construct MPI all sum all operator
template <class T>
sopt::OperatorFunction<T> init_all_sum_all(const sopt::mpi::Communicator &comm) {
  return [=](T &output, const T &input) { output = comm.all_sum_all<T>(input); };
}
#endif
//! constructs lambdas that apply degridding matrix with adjoint
template <class T, class... ARGS>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_gridding_matrix_2d(
    ARGS &&... args) {
  const std::shared_ptr<const Sparse<t_complex>> interpolation_matrix =
      std::make_shared<const Sparse<t_complex>>(
          details::init_gridding_matrix_2d(std::forward<ARGS>(args)...));
  const std::shared_ptr<const Sparse<t_complex>> adjoint =
      std::make_shared<const Sparse<t_complex>>(interpolation_matrix->adjoint());

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
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_zero_padding_2d(
    const Image<typename T::Scalar> &S, const t_real &oversample_ratio) {
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
    for (t_uint j = 0; j < imsizey_; j++) {
      for (t_uint i = 0; i < imsizex_; i++) {
        const t_uint input_index = utilities::sub2ind(j, i, imsizey_, imsizex_);
        const t_uint output_index =
            utilities::sub2ind(y_start + j, x_start + i, ftsizev_, ftsizeu_);
        output(output_index) = S(j, i) * x(input_index);
      }
    }
  };
  auto indirect = [=](T &output, const T &x) {
    assert(x.size() == ftsizeu_ * ftsizev_);
    output = T::Zero(imsizey_ * imsizex_);
#pragma omp parallel for collapse(2)
    for (t_uint j = 0; j < imsizey_; j++) {
      for (t_uint i = 0; i < imsizex_; i++) {
        const t_uint output_index = utilities::sub2ind(j, i, imsizey_, imsizex_);
        const t_uint input_index = utilities::sub2ind(y_start + j, x_start + i, ftsizev_, ftsizeu_);
        output(output_index) = std::conj(S(j, i)) * x(input_index);
      }
    }
  };
  return std::make_tuple(direct, indirect);
}

template <class T>
sopt::OperatorFunction<T> init_normalise(const t_real &op_norm) {
  if (not(op_norm > 0)) throw std::runtime_error("Operator norm is not greater than zero.");
  return [=](T &output, const T &x) { output = x / op_norm; };
}

//! Construsts zero padding operator
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_weights_(
    const Vector<t_complex> &weights) {
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
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_FFT_2d(
    const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_factor_,
    const fftw_plan fftw_plan_flag_ = fftw_plan::measure) {
  t_int const ftsizeu_ = std::floor(imsizex_ * oversample_factor_);
  t_int const ftsizev_ = std::floor(imsizey_ * oversample_factor_);
  t_int plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
  switch (fftw_plan_flag_) {
  case (fftw_plan::measure):
    plan_flag = (FFTW_MEASURE | FFTW_PRESERVE_INPUT);
    break;
  case (fftw_plan::estimate):
    plan_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
    break;
  }

#ifdef PURIFY_OPENMP_FFTW
  PURIFY_LOW_LOG("Using OpenMP threading with FFTW.");
  fftw_init_threads();
#endif
  Vector<typename T::Scalar> src = Vector<t_complex>::Zero(ftsizev_ * ftsizeu_);
  Vector<typename T::Scalar> dst = Vector<t_complex>::Zero(ftsizev_ * ftsizeu_);
  // creating plans
  const auto del = [](fftw_plan_s *plan) { fftw_destroy_plan(plan); };
  // fftw plan with threads needs to be used before each fftw_plan is created
#ifdef PURIFY_OPENMP_FFTW
  fftw_plan_with_nthreads(omp_get_max_threads());
#endif
  const std::shared_ptr<fftw_plan_s> m_plan_forward(
      fftw_plan_dft_2d(ftsizev_, ftsizeu_, reinterpret_cast<fftw_complex *>(src.data()),
                       reinterpret_cast<fftw_complex *>(dst.data()), FFTW_FORWARD, plan_flag),
      del);
  // fftw plan with threads needs to be used before each fftw_plan is created
#ifdef PURIFY_OPENMP_FFTW
  fftw_plan_with_nthreads(omp_get_max_threads());
#endif
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
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_padding_and_FFT_2d(
    const std::function<t_real(t_real)> &ftkernelu, const std::function<t_real(t_real)> &ftkernelv,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &oversample_ratio = 2,
    const fftw_plan &ft_plan = fftw_plan::measure, const t_real &w_mean = 0,
    const t_real &cellx = 1, const t_real &celly = 1) {
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;

  const Image<t_complex> S = purify::details::init_correction2d(
      oversample_ratio, imsizey, imsizex, ftkernelu, ftkernelv, w_mean, cellx, celly);
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
    const t_real &oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const fftw_plan &ft_plan = fftw_plan::measure,
    const bool w_stacking = false, const t_real &cellx = 1, const t_real &celly = 1,
    const bool on_the_fly = true) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  t_real const w_mean = w_stacking ? w.array().mean() : 0.;
  const t_real L = widefield::fov_cosine(cellx, imsizex);
  const t_real M = widefield::fov_cosine(celly, imsizey);
  const t_real dl = L / imsizex;
  const t_real dm = M / imsizey;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, w_mean, dl, dm);
  sopt::OperatorFunction<T> directG, indirectG;
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  PURIFY_MEDIUM_LOG("Mean, w: {}, +/- {}", w_mean, (w.maxCoeff() - w.minCoeff()) * 0.5);
  std::tie(directG, indirectG) =
      (on_the_fly)
          ? purify::operators::init_on_the_fly_gridding_matrix_2d<T>(
                u, v, weights, imsizey, imsizex, oversample_ratio, kernelu, Ju, 4e5)
          : purify::operators::init_gridding_matrix_2d<T>(
                u, v, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, Ju, Jv);
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  return std::make_tuple(direct, indirect);
}

#ifdef PURIFY_MPI
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_mpi_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
    const bool w_stacking = false, const t_real &cellx = 1, const t_real &celly = 1,
    const bool on_the_fly = true) {
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  const t_real L = widefield::fov_cosine(cellx, imsizex);
  const t_real M = widefield::fov_cosine(celly, imsizey);
  const t_real dl = L / imsizex;
  const t_real dm = M / imsizey;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, 0., dl, dm);
  sopt::OperatorFunction<T> directG, indirectG;
  if (w_stacking == true)
    throw std::runtime_error(
        "w-term correction not supported for this measurement operator or MPI method.");
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  std::tie(directG, indirectG) =
      (on_the_fly)
          ? purify::operators::init_on_the_fly_gridding_matrix_2d<T>(
                comm, u, v, weights, imsizey, imsizex, oversample_ratio, kernelu, Ju, 4e5)
          : purify::operators::init_gridding_matrix_2d<T>(
                comm, u, v, weights, imsizey, imsizex, oversample_ratio, kernelv, kernelu, Ju, Jv);
  auto direct = sopt::chained_operators<T>(directG, directFZ);
  auto indirect = sopt::chained_operators<T>(indirectFZ, indirectG);
  PURIFY_LOW_LOG("Finished consturction of Φ.");
  if (comm.is_root())
    return std::make_tuple(direct, indirect);
  else
    return std::make_tuple(directG, indirectG);
}
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>>
base_mpi_all_to_all_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const operators::fftw_plan ft_plan = operators::fftw_plan::measure,
    const bool w_stacking = false, const t_real cellx = 1, const t_real celly = 1) {
  const t_uint number_of_images = comm.size();
  if (std::any_of(image_index.begin(), image_index.end(), [&number_of_images](int index) {
        return index < 0 or index > (number_of_images - 1);
      }))
    throw std::runtime_error("Image index is out of bounds");
  std::function<t_real(t_real)> kernelu, kernelv, ftkernelu, ftkernelv;
  std::tie(kernelu, kernelv, ftkernelu, ftkernelv) =
      purify::create_kernels(kernel, Ju, Jv, imsizey, imsizex, oversample_ratio);
  sopt::OperatorFunction<T> directFZ, indirectFZ;
  t_real const w_mean = w_stacking ? w_stacks.at(comm.rank()) : 0.;
  const t_real L = widefield::fov_cosine(cellx, imsizex);
  const t_real M = widefield::fov_cosine(celly, imsizey);
  const t_real dl = L / imsizex;
  const t_real dm = M / imsizey;
  std::tie(directFZ, indirectFZ) = base_padding_and_FFT_2d<T>(
      ftkernelu, ftkernelv, imsizey, imsizex, oversample_ratio, ft_plan, w_mean, dl, dm);
  sopt::OperatorFunction<T> directG, indirectG;
  PURIFY_MEDIUM_LOG("FoV (width, height): {} deg x {} deg", imsizex * cellx / (60. * 60.),
                    imsizey * celly / (60. * 60.));
  PURIFY_LOW_LOG("Constructing Weighting and MPI Gridding Operators: WG");
  PURIFY_MEDIUM_LOG("Number of visibilities: {}", u.size());
  const t_int local_grid_size =
      std::floor(imsizex * oversample_ratio) * std::floor(imsizex * oversample_ratio);
  std::tie(directG, indirectG) =
      purify::operators::init_gridding_matrix_2d_all_to_all<T, std::int64_t>(
          comm, static_cast<std::int64_t>(local_grid_size),
          static_cast<std::int64_t>(comm.rank()) * static_cast<std::int64_t>(local_grid_size),
          number_of_images, image_index, u, v, weights, imsizey, imsizex, oversample_ratio, kernelv,
          kernelu, Ju, Jv);
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
    const Vector<t_complex> &weights, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_stacking = false,
    const t_real &cellx = 1, const t_real &celly = 1) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_stacking,
      cellx, celly);
  return std::make_shared<sopt::LinearTransform<T>>(directDegrid, M, indirectDegrid, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const utilities::vis_params &uv_vis_input, const t_uint &imsizey, const t_uint &imsizex,
    const t_real &cell_x, const t_real &cell_y, const t_real &oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_stacking = false) {
  const auto uv_vis = utilities::convert_to_pixels(uv_vis_input, cell_x, cell_y, imsizex, imsizey,
                                                   oversample_ratio);
  return init_degrid_operator_2d<T>(uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
                                    oversample_ratio, kernel, Ju, Jv, w_stacking, cell_x, cell_y);
}

#ifdef PURIFY_MPI
//! Returns linear transform that is the weighted degridding operator with mpi all sum all
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_stacking = false, const t_real &cellx = 1, const t_real &celly = 1) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_degrid_operator_2d<T>(
      u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan, w_stacking,
      cellx, celly);
  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real &oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_stacking = false) {
  const auto uv_vis = utilities::convert_to_pixels(uv_vis_input, cell_x, cell_y, imsizex, imsizey,
                                                   oversample_ratio);
  return init_degrid_operator_2d<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                    imsizex, oversample_ratio, kernel, Ju, Jv, w_stacking, cell_x,
                                    cell_y);
}

//! Returns linear transform that is the weighted degridding operator with a distributed Fourier
//! grid
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint &imsizey,
    const t_uint &imsizex, const t_real &oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_stacking = false, const t_real &cellx = 1, const t_real &celly = 1) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  auto Broadcast = purify::operators::init_broadcaster<T>(comm);
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) = purify::operators::base_mpi_degrid_operator_2d<T>(
      comm, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel, Ju, Jv, ft_plan,
      w_stacking, cellx, celly);

  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(Broadcast, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_mpi(
    const sopt::mpi::Communicator &comm, const utilities::vis_params &uv_vis_input,
    const t_uint &imsizey, const t_uint &imsizex, const t_real &cell_x, const t_real &cell_y,
    const t_real oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_stacking = false) {
  const auto uv_vis = utilities::convert_to_pixels(uv_vis_input, cell_x, cell_y, imsizex, imsizey,
                                                   oversample_ratio);
  return init_degrid_operator_2d_mpi<T>(comm, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey,
                                        imsizex, oversample_ratio, kernel, Ju, Jv, w_stacking,
                                        cell_x, cell_y);
}

//! Returns linear transform that is the weighted degridding operator with a distributed Fourier
//! grid
template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_all_to_all(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_real> &w, const Vector<t_complex> &weights, const t_uint imsizey,
    const t_uint imsizex, const t_real oversample_ratio = 2,
    const kernels::kernel kernel = kernels::kernel::kb, const t_uint Ju = 4, const t_uint Jv = 4,
    const bool w_stacking = false, const t_real cellx = 1, const t_real celly = 1) {
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  std::array<t_int, 3> N = {0, 1, static_cast<t_int>(imsizey * imsizex)};
  std::array<t_int, 3> M = {0, 1, static_cast<t_int>(u.size())};
  auto Broadcast = purify::operators::init_broadcaster<T>(comm);
  sopt::OperatorFunction<T> directDegrid, indirectDegrid;
  std::tie(directDegrid, indirectDegrid) =
      purify::operators::base_mpi_all_to_all_degrid_operator_2d<T>(
          comm, image_index, w_stacks, u, v, w, weights, imsizey, imsizex, oversample_ratio, kernel,
          Ju, Jv, ft_plan, w_stacking, cellx, celly);

  const auto allsumall = purify::operators::init_all_sum_all<T>(comm);
  auto direct = directDegrid;
  auto indirect = sopt::chained_operators<T>(allsumall, indirectDegrid);
  return std::make_shared<sopt::LinearTransform<T>>(direct, M, indirect, N);
}

template <class T>
std::shared_ptr<sopt::LinearTransform<T>> init_degrid_operator_2d_all_to_all(
    const sopt::mpi::Communicator &comm, const std::vector<t_int> &image_index,
    const std::vector<t_real> &w_stacks, const utilities::vis_params &uv_vis_input,
    const t_uint imsizey, const t_uint imsizex, const t_real cell_x, const t_real cell_y,
    const t_real oversample_ratio = 2, const kernels::kernel kernel = kernels::kernel::kb,
    const t_uint Ju = 4, const t_uint Jv = 4, const bool w_stacking = false) {
  const auto uv_vis = utilities::convert_to_pixels(uv_vis_input, cell_x, cell_y, imsizex, imsizey,
                                                   oversample_ratio);
  return init_degrid_operator_2d_all_to_all<T>(
      comm, image_index, w_stacks, uv_vis.u, uv_vis.v, uv_vis.w, uv_vis.weights, imsizey, imsizex,
      oversample_ratio, kernel, Ju, Jv, w_stacking, cell_x, cell_y);
}
#endif

}  // namespace measurementoperator

};  // namespace purify
#endif
