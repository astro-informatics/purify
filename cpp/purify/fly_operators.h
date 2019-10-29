#ifndef PURIFY_FLY_OPERATORS_H
#define PURIFY_FLY_OPERATORS_H

#include "purify/config.h"
#include "purify/types.h"
#include <set>
#include "purify/operators.h"

#ifdef PURIFY_MPI
#include "purify/AllToAllSparseVector.h"
#include "purify/DistributeSparseVector.h"
#include "purify/IndexMapping.h"
#include "purify/mpi_utilities.h"
#include <sopt/mpi/communicator.h>
#endif

namespace purify {
namespace operators {
//! on the fly application of the degridding operator using presampling
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_on_the_fly_gridding_matrix_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_complex> &weights,
    const t_uint &imsizey_, const t_uint &imsizex_, const t_real &oversample_ratio,
    const std::function<t_real(t_real)> &kernelu, const t_uint Ju, const t_int total_samples) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  const std::shared_ptr<Vector<t_real>> u_ptr = std::make_shared<Vector<t_real>>(u);
  const std::shared_ptr<Vector<t_real>> v_ptr = std::make_shared<Vector<t_real>>(v);
  const std::shared_ptr<Vector<t_complex>> weights_ptr =
      std::make_shared<Vector<t_complex>>(weights);
  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  const t_int jv_max = std::min(Ju, ftsizev_);
  const auto samples = kernels::kernel_samples(
      total_samples, [&](const t_real x) { return kernelu(x * ju_max * 0.5); });
  std::set<t_int> nonZeros_set;
  for (t_int m = 0; m < rows; ++m) {
    t_complex result = 0;
    const t_real u_val = (*u_ptr)(m);
    const t_real v_val = (*v_ptr)(m);
    const t_real k_u = std::floor(u_val - ju_max * 0.5);
    const t_real k_v = std::floor(v_val - jv_max * 0.5);
    for (t_int jv = 1; jv < jv_max + 1; ++jv) {
      const t_uint p = utilities::mod(k_v + jv, ftsizev_);
      const t_real c_0 =
          static_cast<t_int>(std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
      for (t_int ju = 1; ju < ju_max + 1; ++ju) {
        const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
        const t_int i_0 = static_cast<t_int>(
            std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
        const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        nonZeros_set.insert(index);
      }
    }
  }

  std::vector<t_int> nonZeros_vec(nonZeros_set.begin(), nonZeros_set.end());
  std::sort(nonZeros_vec.data(), nonZeros_vec.data() + nonZeros_vec.size());
  SparseVector<t_int> mapping(ftsizev_ * ftsizeu_);
  mapping.reserve(nonZeros_vec.size());
  for (t_int index = 0; index < nonZeros_vec.size(); index++)
    mapping.coeffRef(nonZeros_vec[index]) = index;
  PURIFY_LOW_LOG("Non Zero grid locations: {} ", mapping.nonZeros());

  const auto degrid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                       ftsizeu_, ftsizev_](T &output, const T &input) {
    output = T::Zero(u_ptr->size());
    assert(input.size() == ftsizeu_ * ftsizev_);
#ifdef PURIFY_OPENMP
#pragma omp parallel for
#endif
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
          const t_real sign = kernelu_val * kernelv_val;
          result += input(index) * sign;
        }
      }
      output(m) = result;
    }
    output.array() *= (*weights_ptr).array();
  };

  const auto grid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                     ftsizeu_, ftsizev_, mapping, nonZeros_vec](T &output, const T &input) {
    const t_int N = ftsizeu_ * ftsizev_;
    output = T::Zero(N);
#ifdef PURIFY_OPENMP
    t_int const max_threads = omp_get_max_threads();
#else
    t_int const max_threads = 1;
#endif
    T output_compressed = T::Zero(nonZeros_vec.size() * max_threads);
    assert(output.size() == N);
#ifdef PURIFY_OPENMP
#pragma omp parallel for
#endif
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
#ifdef PURIFY_OPENMP
      const t_int shift = omp_get_thread_num() * nonZeros_vec.size();
#else
      const t_int shift = 0;
#endif
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      const t_complex vis = input(m) * std::conj((*weights_ptr)(m));
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const t_int index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
          const t_complex result = kernelu_val * kernelv_val * vis;
          output_compressed(mapping.coeff(index) + shift) += result;
        }
      }
    }
    for (t_int m = 1; m < max_threads; m++) {
      const t_int loop_shift = m * nonZeros_vec.size();
      output_compressed.segment(0, nonZeros_vec.size()) +=
          output_compressed.segment(loop_shift, nonZeros_vec.size());
    }
    for (t_int index = 0; index < nonZeros_vec.size(); index++)
      output(nonZeros_vec[index]) += output_compressed(index);
  };
  return std::make_tuple(degrid, grid);
}
#ifdef PURIFY_MPI
//! on the fly gridding matrix application with compression
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_on_the_fly_gridding_matrix_2d(
    const sopt::mpi::Communicator &comm, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_complex> &weights, const t_uint &imsizey_, const t_uint &imsizex_,
    const t_real &oversample_ratio, const std::function<t_real(t_real)> &kernelu, const t_uint Ju,
    const t_int total_samples) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  const std::shared_ptr<Vector<t_real>> u_ptr = std::make_shared<Vector<t_real>>(u);
  const std::shared_ptr<Vector<t_real>> v_ptr = std::make_shared<Vector<t_real>>(v);
  const std::shared_ptr<Vector<t_complex>> weights_ptr =
      std::make_shared<Vector<t_complex>>(weights);
  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  const t_int jv_max = std::min(Ju, ftsizev_);
  const auto samples = kernels::kernel_samples(
      total_samples, [&](const t_real x) { return kernelu(x * ju_max * 0.5); });

  std::set<t_int> nonZeros_set;
  for (t_int m = 0; m < rows; ++m) {
    t_complex result = 0;
    const t_real u_val = (*u_ptr)(m);
    const t_real v_val = (*v_ptr)(m);
    const t_real k_u = std::floor(u_val - ju_max * 0.5);
    const t_real k_v = std::floor(v_val - jv_max * 0.5);
    for (t_int jv = 1; jv < jv_max + 1; ++jv) {
      const t_uint p = utilities::mod(k_v + jv, ftsizev_);
      const t_real c_0 =
          static_cast<t_int>(std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
      for (t_int ju = 1; ju < ju_max + 1; ++ju) {
        const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
        const t_int i_0 = static_cast<t_int>(
            std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
        const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        nonZeros_set.insert(index);
      }
    }
  }

  const std::shared_ptr<DistributeSparseVector> distributor =
      std::make_shared<DistributeSparseVector>(nonZeros_set, cols, comm);

  std::vector<t_int> nonZeros_vec(nonZeros_set.begin(), nonZeros_set.end());
  std::sort(nonZeros_vec.data(), nonZeros_vec.data() + nonZeros_vec.size());
  SparseVector<t_int> mapping(cols);
  mapping.reserve(nonZeros_vec.size());
  for (t_int index = 0; index < nonZeros_vec.size(); index++)
    mapping.coeffRef(nonZeros_vec[index]) = index;
  PURIFY_LOW_LOG("Non Zero grid locations: {} ", mapping.nonZeros());
  const t_int nonZeros_size = mapping.nonZeros();
  const auto degrid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                       ftsizeu_, ftsizev_, distributor, mapping, nonZeros_size,
                       comm](T &output, const T &input) {
    T input_buff;
    if (comm.is_root()) {
      assert(input.size() == ftsizeu_ * ftsizev_);
      distributor->scatter(input, input_buff);
    } else {
      distributor->scatter(input_buff);
    }
    assert(input_buff.size() == mapping.nonZeros());
#pragma omp parallel for
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
          const t_real sign = kernelu_val * kernelv_val;
          result += input_buff(mapping.coeff(index)) * sign;
        }
      }
      output(m) = result;
    }
    output.array() *= (*weights_ptr).array();
  };
  const auto grid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                     ftsizeu_, ftsizev_, mapping, nonZeros_size, distributor,
                     comm](T &output, const T &input) {
    const t_int N = ftsizeu_ * ftsizev_;
#ifdef PURIFY_OPENMP
    t_int const max_threads = omp_get_max_threads();
#else
    t_int const max_threads = 1;
#endif
    T output_compressed = T::Zero(nonZeros_size * max_threads);
#pragma omp parallel for
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
#ifdef PURIFY_OPENMP
      const t_int shift = omp_get_thread_num() * nonZeros_size;
#else
      const t_int shift = 0;
#endif
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      const t_complex vis = input(m) * std::conj((*weights_ptr)(m));
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const t_int index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
          const t_complex result = kernelu_val * kernelv_val * vis;
          output_compressed(mapping.coeff(index) + shift) += result;
        }
      }
    }
    T output_sum = T::Zero(nonZeros_size);
    for (t_int m = 0; m < max_threads; m++) {
      const t_int loop_shift = m * nonZeros_size;
      output_sum += output_compressed.segment(loop_shift, nonZeros_size);
    }
    if (not comm.is_root()) {
      distributor->gather(output_sum);
    } else {
      output = T::Zero(N);
      distributor->gather(output_sum, output);
      assert(output.size() == N);
    }
  };
  return std::make_tuple(degrid, grid);
}

//! Construct all to all gridding matrix
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_on_the_fly_gridding_matrix_2d(
    const sopt::mpi::Communicator &comm, const t_uint number_of_images,
    const std::vector<t_int> &image_index, const Vector<t_real> &u, const Vector<t_real> &v,
    const Vector<t_complex> &weights, const t_uint &imsizey_, const t_uint &imsizex_,
    const t_real &oversample_ratio, const std::function<t_real(t_real)> &kernelu, const t_uint Ju,
    const t_int total_samples) {
  if (std::any_of(image_index.begin(), image_index.end(), [&number_of_images](int index) {
        return index < 0 or index > (number_of_images - 1);
      }))
    throw std::runtime_error("Image index is out of bounds");
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  const std::shared_ptr<Vector<t_real>> u_ptr = std::make_shared<Vector<t_real>>(u);
  const std::shared_ptr<Vector<t_real>> v_ptr = std::make_shared<Vector<t_real>>(v);
  const std::shared_ptr<Vector<t_complex>> weights_ptr =
      std::make_shared<Vector<t_complex>>(weights);
  const std::shared_ptr<std::vector<t_int>> image_index_ptr =
      std::make_shared<std::vector<t_int>>(image_index);
  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  const t_int jv_max = std::min(Ju, ftsizev_);
  const auto samples = kernels::kernel_samples(
      total_samples, [&](const t_real x) { return kernelu(x * ju_max * 0.5); });

  std::set<std::int64_t> nonZeros_set;
  for (t_int m = 0; m < rows; ++m) {
    t_complex result = 0;
    const t_real u_val = (*u_ptr)(m);
    const t_real v_val = (*v_ptr)(m);
    const t_real k_u = std::floor(u_val - ju_max * 0.5);
    const t_real k_v = std::floor(v_val - jv_max * 0.5);
    for (t_int jv = 1; jv < jv_max + 1; ++jv) {
      const t_uint p = utilities::mod(k_v + jv, ftsizev_);
      const t_real c_0 =
          static_cast<t_int>(std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
      for (t_int ju = 1; ju < ju_max + 1; ++ju) {
        const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
        const t_int i_0 = static_cast<t_int>(
            std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
        const std::int64_t index =
            static_cast<std::int64_t>(utilities::sub2ind(p, q, ftsizev_, ftsizeu_)) +
            static_cast<std::int64_t>((*image_index_ptr)[m]) *
                static_cast<std::int64_t>(ftsizev_ * ftsizeu_);
        nonZeros_set.insert(index);
      }
    }
  }

  const AllToAllSparseVector<std::int64_t> distributor(
      nonZeros_set, ftsizeu_ * ftsizev_,
      static_cast<std::int64_t>(comm.rank()) * static_cast<std::int64_t>(ftsizeu_ * ftsizev_),
      comm);

  std::vector<std::int64_t> nonZeros_vec(nonZeros_set.begin(), nonZeros_set.end());
  std::sort(nonZeros_vec.data(), nonZeros_vec.data() + nonZeros_vec.size());
  SparseVector<t_int, std::int16_t> mapping(static_cast<std::int64_t>(ftsizev_ * ftsizeu_) *
                                            static_cast<std::int64_t>(number_of_images));
  mapping.reserve(nonZeros_vec.size());
  for (t_int index = 0; index < nonZeros_vec.size(); index++)
    mapping.coeffRef(nonZeros_vec[index]) = index;
  PURIFY_LOW_LOG("Non Zero grid locations: {} ", mapping.nonZeros());

  const auto degrid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                       ftsizeu_, ftsizev_, distributor, mapping, image_index_ptr,
                       comm](T &output, const T &input) {
    assert(input.size() == ftsizeu_ * ftsizev_);
    T input_buff;
    distributor.recv_grid(input, input_buff);
#pragma omp parallel for
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const std::int64_t index =
              static_cast<std::int64_t>(utilities::sub2ind(p, q, ftsizev_, ftsizeu_)) +
              static_cast<std::int64_t>((*image_index_ptr)[m]) *
                  static_cast<std::int64_t>(ftsizev_ * ftsizeu_);
          const t_real sign = kernelu_val * kernelv_val;
          result += input_buff(mapping.coeff(index)) * sign;
        }
      }
      output(m) = result;
    }
    output.array() *= (*weights_ptr).array();
  };
  const t_int nonZeros_size = mapping.nonZeros();
  const auto grid = [rows, ju_max, jv_max, I, u_ptr, v_ptr, weights_ptr, samples, total_samples,
                     ftsizeu_, ftsizev_, mapping, nonZeros_size, distributor, image_index_ptr,
                     comm](T &output, const T &input) {
    const t_int N = ftsizeu_ * ftsizev_;
    output = T::Zero(N);
#ifdef PURIFY_OPENMP
    t_int const max_threads = omp_get_max_threads();
#else
    t_int const max_threads = 1;
#endif
    T output_compressed = T::Zero(nonZeros_size * max_threads);
    assert(output.size() == N);
#pragma omp parallel for
    for (t_int m = 0; m < rows; ++m) {
      t_complex result = 0;
#ifdef PURIFY_OPENMP
      const t_int shift = omp_get_thread_num() * nonZeros_size;
#else
      const t_int shift = 0;
#endif
      const t_real u_val = (*u_ptr)(m);
      const t_real v_val = (*v_ptr)(m);
      const t_real k_u = std::floor(u_val - ju_max * 0.5);
      const t_real k_v = std::floor(v_val - jv_max * 0.5);
      const t_complex vis = input(m) * std::conj((*weights_ptr)(m));
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_real c_0 = static_cast<t_int>(
            std::floor(2 * std::abs(v_val - (k_v + jv)) * total_samples / jv_max));
        assert(c_0 >= 0);
        assert(c_0 < total_samples);
        const t_real kernelv_val = samples[c_0] * (1. - (2 * (p % 2)));
        for (t_int ju = 1; ju < ju_max + 1; ++ju) {
          const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
          const t_int i_0 = static_cast<t_int>(
              std::floor(2 * std::abs(u_val - (k_u + ju)) * total_samples / ju_max));
          assert(i_0 >= 0);
          assert(i_0 < total_samples);
          const t_real kernelu_val = samples[i_0] * (1. - (2 * (q % 2)));
          const std::int64_t index =
              static_cast<std::int64_t>(utilities::sub2ind(p, q, ftsizev_, ftsizeu_)) +
              static_cast<std::int64_t>((*image_index_ptr)[m]) *
                  static_cast<std::int64_t>(ftsizev_ * ftsizeu_);
          const t_complex result = kernelu_val * kernelv_val * vis;
          output_compressed(mapping.coeff(index) + shift) += result;
        }
      }
    }
    for (t_int m = 1; m < max_threads; m++) {
      const t_int loop_shift = m * nonZeros_size;
      output_compressed.segment(0, nonZeros_size) +=
          output_compressed.segment(loop_shift, nonZeros_size);
    }
    distributor.send_grid(output_compressed.segment(0, nonZeros_size), output);
  };
  return std::make_tuple(degrid, grid);
}
#endif

}  // namespace operators
}  // namespace purify

#endif
