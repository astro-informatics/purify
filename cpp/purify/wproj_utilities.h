#ifndef PURIFY_wproj_utilities_H
#define PURIFY_wproj_utilities_H

#include "purify/config.h"
#include <fstream>
#include <iostream>
#ifdef PURIFY_OPENMP
#include <omp.h>
#endif
#include <random>
#include <set>
#include <stdio.h>
#include <string>
#include <time.h>
#include <Eigen/Sparse>
#include <sys/stat.h>
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"
#include "purify/utilities.h"

namespace purify {
namespace utilities {
//! Sort visibilities to be from w_max to w_min
vis_params sort_by_w(const vis_params &uv_data);
//! Calculate W Lambert function
template <typename T> t_real w_lambert(T x, const t_real &relative_difference);
} // namespace utilities
namespace wproj_utilities {
//! Work out max L and M directional cosines from image parameters
std::tuple<t_real, t_real>
fov_cosines(t_real const &cell_x, t_real const &cell_y, t_uint const &x_size, t_uint const &y_size);
//! Convert from dense to sparse
template <typename T>
Sparse<t_complex> convert_sparse(const Eigen::MatrixBase<T> &M, const t_real &threshold = 0.);
//! Generate image of DDE for A or W projection
Matrix<t_complex> generate_dde(const std::function<t_complex(t_real, t_real)> &dde,
                               const t_real &cell_x, const t_real &cell_y, const t_uint &x_size,
                               const t_uint &y_size);
//! Generates image of chirp
Matrix<t_complex> generate_chirp(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_uint &x_size, const t_uint &y_size);
Matrix<t_complex> generate_chirp(const std::function<t_complex(t_real, t_real)> &dde,
                                 const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_uint &x_size, const t_uint &y_size);
//! Generates row of chirp matrix from image of chirp
Sparse<t_complex> create_chirp_row(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                   const t_uint &ftsizev, const t_uint &ftsizeu,
                                   const t_real &energy_fraction,
                                   const sopt::OperatorFunction<Vector<t_complex>> &fftop);
Sparse<t_complex> create_chirp_row(const Vector<t_complex> &chirp_image,
                                   const t_real &energy_fraction,
                                   const sopt::OperatorFunction<Vector<t_complex>> &fftop);
//! Returns threshold to keep a fraction of energy in the sparse row
template <typename T>
t_real sparsify_row_thres(const Eigen::SparseMatrixBase<T> &row, const t_real &energy);
//! Returns threshold to keep a fraction of energy in the dense row
template <typename T>
t_real sparsify_row_dense_thres(const Eigen::MatrixBase<T> &row, const t_real &energy);
//! Perform convolution with gridding matrix row and chirp matrix row
template <class T>
Sparse<t_complex> row_wise_convolution(const Sparse<t_complex> &Grid_, const Sparse<T> &chirp_,
                                       const t_uint &x_size, const t_uint &y_size);
//! Produce Gridding matrix convovled with chirp matrix for wprojection
Sparse<t_complex>
wprojection_matrix(const Sparse<t_complex> &G, const t_uint &x_size, const t_uint &y_size,
                   const Vector<t_real> &w_components, const t_real &cell_x, const t_real &cell_y,
                   const t_real &energy_fraction_chirp, const t_real &energy_fraction_wproj,
                   const expansions::series series = expansions::series::none,
                   const t_uint order = 1, const t_real &interpolation_error = 1e-2);
//! SNR calculation
t_real snr_metric(const Image<t_real> &model, const Image<t_real> &solution);
//! MR calculation
t_real mr_metric(const Image<t_real> &model, const Image<t_real> &solution);
//! return fraction of non zero values from sparse matrix
t_real sparsity_sp(const Sparse<t_complex> &Gmat);
//! return faction of non zero values from matrix
t_real sparsity_im(const Image<t_complex> &Cmat);
//! Genereates an image of a Gaussian as a sparse matrice
Sparse<t_complex>
generate_vect(const t_uint &x_size, const t_uint &y_size, const t_real &sigma, const t_real &mean);
//! Calculate upsample ratio from bandwidth (only needed for simulations)
t_real upsample_ratio_sim(const utilities::vis_params &uv_vis, const t_real &L, const t_real &M,
                          const t_int &x_size, const t_int &y_size, const t_int &multipleOf);

template <typename T>
Sparse<t_complex> convert_sparse(const Eigen::MatrixBase<T> &M, const t_real &threshold) {
  const Sparse<t_complex> M_sparse
      = M.sparseView(std::max(M.cwiseAbs().maxCoeff() * 1e-10, threshold), 1);
  Sparse<t_complex> out(M.rows(), M.cols());
  out.reserve(M_sparse.nonZeros());
  for(t_int k = 0; k < M_sparse.outerSize(); ++k)
    for(Sparse<t_complex>::InnerIterator it(M_sparse, k); it; ++it) {
      out.coeffRef(it.row(), it.col()) = it.value();
    }
  return out;
}
template <typename T>
t_real sparsify_row_thres(const Eigen::SparseMatrixBase<T> &row, const t_real &energy) {
  /*
     Takes in a row of G and returns indexes of coeff to keep in the row sparse version
     energy:: how much energy - in l2 sens - to keep after hard-thresholding
     */

  if(energy >= 1)
    return 0;

  const Sparse<t_real> row_abs = row.cwiseAbs();

  t_real abs_row_max = 0;
  for(t_uint i = 0; i < row_abs.nonZeros(); i++) {
    if(*(row_abs.valuePtr() + i) > abs_row_max)
      abs_row_max = *(row_abs.valuePtr() + i);
  }
  const t_real row_total_energy = row_abs.cwiseProduct(row_abs).sum();
  const t_real target_threshold_energy_sum = row_total_energy * energy;
  const t_int niters = 200;
  t_real lower = 0;
  t_real upper = abs_row_max;
  t_real current_threshold = upper * 0.5;
  t_real old_threshold = upper;
  bool converged = false;
  for(t_int i = 0; i < niters; i++) {
    t_real threshold_energy_sum = 0;
    for(t_int j = 0; j < row_abs.nonZeros(); j++) {
      if(*(row_abs.valuePtr() + j) > current_threshold)
        threshold_energy_sum += *(row_abs.valuePtr() + j) * *(row_abs.valuePtr() + j);
    }
    if((std::abs(current_threshold - old_threshold) / std::abs(old_threshold) < 1e-3)
       and ((threshold_energy_sum >= target_threshold_energy_sum)
            and (((threshold_energy_sum / target_threshold_energy_sum) - 1) < 0.01))) {
      converged = true;
      break;
    }
    if(threshold_energy_sum < target_threshold_energy_sum)
      upper = current_threshold;
    else
      lower = current_threshold;
    old_threshold = current_threshold;
    current_threshold = (lower + upper) * 0.5;
  }
  if(!converged)
    current_threshold = lower;
  return current_threshold;
}
template <typename T>
t_real sparsify_row_dense_thres(const Eigen::MatrixBase<T> &row, const t_real &energy) {
  /*
     Takes in a row of G and returns indexes of coeff to keep in the row sparse version
     energy:: how much energy - in l2 sens - to keep after hard-thresholding
     */

  if(energy >= 1)
    return 0;

  const Vector<t_real> row_abs = row.cwiseAbs();
  const t_real abs_row_max = row.cwiseAbs().maxCoeff();
  const t_real row_total_energy = row_abs.cwiseProduct(row_abs).sum();
  const t_real target_threshold_energy_sum = row_total_energy * energy;
  const t_int niters = 200;
  t_real lower = 0;
  t_real upper = abs_row_max;
  t_real current_threshold = upper * 0.5;
  t_real old_threshold = upper;
  bool converged = false;
  for(t_int i = 0; i < niters; i++) {
    t_real threshold_energy_sum = 0;
    for(t_int j = 0; j < row_abs.size(); j++) {
      if(row_abs(j) > current_threshold)
        threshold_energy_sum += row_abs(j) * row_abs(j);
    }
    if((std::abs(current_threshold - old_threshold) / std::abs(old_threshold) < 1e-3)
       and ((threshold_energy_sum >= target_threshold_energy_sum)
            and (((threshold_energy_sum / target_threshold_energy_sum) - 1) < 0.01))) {
      converged = true;
      break;
    }
    if(threshold_energy_sum < target_threshold_energy_sum)
      upper = current_threshold;
    else
      lower = current_threshold;
    old_threshold = current_threshold;
    current_threshold = (lower + upper) * 0.5;
  }
  if(!converged)
    current_threshold = lower;
  return current_threshold;
}
template <class T>
Sparse<t_complex> row_wise_convolution(const Sparse<t_complex> &Grid_, const Sparse<T> &chirp_,
                                       const t_uint &x_size, const t_uint &y_size) {
  assert((Grid_.nonZeros() > 0) and (chirp_.nonZeros() > 0));
  Matrix<t_complex> output_row = Matrix<t_complex>::Zero(1, x_size * y_size);
  const t_int x_sizec = std::floor(x_size * 0.5);
  const t_int y_sizec = std::floor(y_size * 0.5);
  for(t_int k = 0; k < output_row.size(); k++) {
    // shifting indicies to workout linear convolution
    t_int i_shift_W = 0;
    t_int j_shift_W = 0;
    std::tie(j_shift_W, i_shift_W) = utilities::ind2sub(k, y_size, x_size);
    const t_int i_W = utilities::mod(i_shift_W + x_sizec, x_size);
    const t_int j_W = utilities::mod(j_shift_W + y_sizec, y_size);
    for(Sparse<t_complex>::InnerIterator it(Grid_, 0); it; ++it) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(it.index(), y_size, x_size);
      const t_int i_G = utilities::mod(i_shift_G + x_sizec, x_size);
      const t_int j_G = utilities::mod(j_shift_G + y_sizec, y_size);
      const t_int i_C = i_W - i_G;
      const t_int j_C = j_W - j_G;
      // Checking that convolution result is going to be within bounds
      if(-x_sizec <= i_C and i_C < std::ceil(x_size * 0.5) and -y_sizec <= j_C
         and j_C < std::ceil(y_size * 0.5)) {
        // FFTshift of result to go into gridding matrix
        const t_int i_shift_C = utilities::mod(i_C, x_size);
        const t_int j_shift_C = utilities::mod(j_C, y_size);
        const t_int pos = utilities::sub2ind(j_shift_C, i_shift_C, y_size, x_size);
        assert(0 <= pos and pos < x_size * y_size);
        output_row(0, k) += it.value() * chirp_.coeff(0, pos);
      }
    }
  }
  return convert_sparse(output_row);
}
template <class T>
Sparse<t_complex>
row_wise_sparse_convolution(const Sparse<t_complex> &Grid_, const Sparse<T> &chirp_,
                            const t_uint &x_size, const t_uint &y_size) {
  assert((Grid_.nonZeros() > 0) and (chirp_.nonZeros() > 0));
  const t_int x_sizec = std::floor(x_size * 0.5);
  const t_int y_sizec = std::floor(y_size * 0.5);
  std::set<t_int> non_zero_W;
  for(Sparse<t_complex>::InnerIterator jt(chirp_, 0); jt; ++jt) {
    // shifting indicies to workout linear convolution
    t_int i_shift_C = 0;
    t_int j_shift_C = 0;
    std::tie(j_shift_C, i_shift_C) = utilities::ind2sub(jt.index(), y_size, x_size);
    const t_int i_C = utilities::mod(i_shift_C + x_sizec, x_size) - x_sizec;
    const t_int j_C = utilities::mod(j_shift_C + y_sizec, y_size) - y_sizec;
    for(Sparse<t_complex>::InnerIterator it(Grid_, 0); it; ++it) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(it.index(), y_size, x_size);
      const t_int i_G = utilities::mod(i_shift_G + x_sizec, x_size) - x_sizec;
      const t_int j_G = utilities::mod(j_shift_G + y_sizec, y_size) - y_sizec;
      const t_int i_W = i_G + i_C;
      const t_int j_W = j_G + j_C;
      if(-x_sizec <= i_W and i_W < std::ceil(x_size * 0.5) and -y_sizec <= j_W
         and j_W < std::ceil(y_size * 0.5)) {
        const t_int i_shift_W = utilities::mod(i_W, x_size);
        const t_int j_shift_W = utilities::mod(j_W, y_size);
        const t_real pos = utilities::sub2ind(j_shift_W, i_shift_W, y_size, x_size);
        non_zero_W.insert(pos);
      }
    }
  }
  if(non_zero_W.size() < std::min(Grid_.nonZeros(), chirp_.nonZeros()))
    throw std::runtime_error("Gridding kernel or chirp kernel is zero.");
  Sparse<t_complex> output_row(1, x_size * y_size);
  output_row.reserve(non_zero_W.size());

  for(t_int k = 0; k < output_row.size(); k++) {
    // shifting indicies to workout linear convolution
    t_int i_shift_W = 0;
    t_int j_shift_W = 0;
    std::tie(j_shift_W, i_shift_W) = utilities::ind2sub(k, y_size, x_size);
    const t_int i_W = utilities::mod(i_shift_W + x_sizec, x_size);
    const t_int j_W = utilities::mod(j_shift_W + y_sizec, y_size);
    for(Sparse<t_complex>::InnerIterator it(Grid_, 0); it; ++it) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(it.index(), y_size, x_size);
      const t_int i_G = utilities::mod(i_shift_G + x_sizec, x_size);
      const t_int j_G = utilities::mod(j_shift_G + y_sizec, y_size);
      const t_int i_C = i_W - i_G;
      const t_int j_C = j_W - j_G;
      // Checking that convolution result is going to be within bounds
      if(-x_sizec <= i_C and i_C < std::ceil(x_size * 0.5) and -y_sizec <= j_C
         and j_C < std::ceil(y_size * 0.5)) {
        // FFTshift of result to go into gridding matrix
        const t_int i_shift_C = utilities::mod(i_C, x_size);
        const t_int j_shift_C = utilities::mod(j_C, y_size);
        const t_int pos = utilities::sub2ind(j_shift_C, i_shift_C, y_size, x_size);
        assert(0 <= pos and pos < x_size * y_size);
        output_row.coeffRef(0, k) += it.value() * chirp_.coeff(0, pos);
      }
    }
  }
  return output_row;
}
} // namespace wproj_utilities
namespace utilities {

template <typename T> t_real w_lambert(const T &x, const t_real &relative_difference = 1e-8) {

  if(std::is_integral<T>::value)
    return w_lambert<t_real>(x, relative_difference);
  T z = T(x);
  T diff = T(1e4 * relative_difference);
  if(x < -1 / std::exp(1))
    throw std::runtime_error("Out of bounds for W lambert function!");
  while(std::abs(diff) > relative_difference) {
    const T f = z * std::exp(z) - x;
    const T w = z - f / (std::exp(z) * (z + 1) - f * (z + 2) / (2 * z + 2));
    diff = w - z;
    z = w;
  }
  return z;
}

} // namespace utilities
} // namespace purify

#endif
