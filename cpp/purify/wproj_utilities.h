#ifndef PURIFY_wproj_utilities_H
#define PURIFY_wproj_utilities_H

#include "purify/config.h"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <set>
#include <stdio.h>
#include <string>
#include <time.h>
#include <Eigen/Sparse>
#include <sys/stat.h>
#include "purify/FFTOperator.h"
#include "purify/logging.h"
#include "purify/types.h"
#include "purify/utilities.h"

namespace purify {
namespace wproj_utilities {
//! Generates image of chirp
Matrix<t_complex> generate_chirp(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_int &x_size, const t_int &y_size);
//! Generates row of chirp matrix from image of chirp
Sparse<t_complex> create_chirp_row(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                   const t_real &ftsizev, const t_real &ftsizeu,
                                   const t_real &energy_fraction,
                                   const std::shared_ptr<FFTOperator> &fftop_);
//! Returns threshold to keep a fraction of energy in the sparse row
template <typename T>
t_real sparsify_row_thres(const Eigen::SparseMatrixBase<T> &row, const t_real &energy);
//! Returns threshold to keep a fraction of energy in the dense row
template <typename T>
t_real sparsify_row_dense_thres(const Eigen::MatrixBase<T> &row, const t_real &energy);
//! Perform convolution with gridding matrix row and chirp matrix row
template <class T>
Sparse<t_complex> row_wise_convolution(const Sparse<t_complex> &Grid_, const Sparse<T> &chirp_,
                                       const t_int &Nx, const t_int &Ny);
//! Produce Gridding matrix convovled with chirp matrix for wprojection
Sparse<t_complex> wprojection_matrix(const Sparse<t_complex> &G, const t_int &Nx, const t_int &Ny,
                                     const Vector<t_real> &w_components, const t_real &cell_x,
                                     const t_real &cell_y, const t_real &energy_fraction_chirp,
                                     const t_real &energy_fraction_wproj);
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
generate_vect(const t_int &x_size, const t_int &y_size, const t_real &sigma, const t_real &mean);
//! Calculate upsample ratio from bandwidth (only needed for simulations)
t_real upsample_ratio_sim(const utilities::vis_params &uv_vis, const t_real &L, const t_real &M,
                          const t_int &x_size, const t_int &y_size, const t_int &multipleOf);

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
                                       const t_int &Nx, const t_int &Ny) {

  assert((Grid_.nonZeros() > 0) and (chirp_.nonZeros() > 0));
  Vector<t_complex> output_row = Vector<t_complex>::Zero(Nx * Ny);
  const t_int Nxc = std::floor(Nx * 0.5);
  const t_int Nyc = std::floor(Ny * 0.5);
  for(t_int k = 0; k < output_row.size(); k++) {
    // shifting indicies to workout linear convolution
    t_int i_shift_W = 0;
    t_int j_shift_W = 0;
    std::tie(j_shift_W, i_shift_W) = utilities::ind2sub(k, Ny, Nx);
    const t_int i_W = utilities::mod(i_shift_W + Nxc, Nx);
    const t_int j_W = utilities::mod(j_shift_W + Nyc, Ny);
    for(t_uint pix = 0; pix < Grid_.nonZeros(); pix++) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(*(Grid_.outerIndexPtr() + pix), Ny, Nx);
      const t_int i_G = utilities::mod(i_shift_G + Nxc, Nx);
      const t_int j_G = utilities::mod(j_shift_G + Nyc, Ny);
      const t_int i_C = i_W - i_G;
      const t_int j_C = j_W - j_G;
      // Checking that convolution result is going to be within bounds
      if(-Nxc <= i_C and i_C < std::ceil(Nx * 0.5) and -Nyc <= j_C and j_C < std::ceil(Ny * 0.5)) {
        // FFTshift of result to go into gridding matrix
        const t_int i_shift_C = utilities::mod(i_C, Nx);
        const t_int j_shift_C = utilities::mod(j_C, Ny);
        const t_int pos = utilities::sub2ind(j_shift_C, i_shift_C, Ny, Nx);
        assert(0 <= pos and pos < Nx * Ny);
        output_row(k) += *(Grid_.valuePtr() + pix) * chirp_.coeff(pos, 0);
      }
    }
  }
  return output_row.sparseView(output_row.cwiseAbs().maxCoeff(), 1e-10);
}
template <class T>
Sparse<t_complex>
row_wise_sparse_convolution(const Sparse<t_complex> &Grid_, const Sparse<T> &chirp_,
                            const t_int &Nx, const t_int &Ny) {
  assert((Grid_.nonZeros() > 0) and (chirp_.nonZeros() > 0));
  const t_int Nxc = std::ceil(Nx * 0.5);
  const t_int Nyc = std::ceil(Ny * 0.5);
  std::set<t_int> non_zero_W;
  for(t_int k = 0; k < chirp_.nonZeros(); k++) {
    // shifting indicies to workout linear convolution
    t_int i_shift_C = 0;
    t_int j_shift_C = 0;
    std::tie(j_shift_C, i_shift_C) = utilities::ind2sub(*(chirp_.outerIndexPtr() + k), Ny, Nx);
    const t_int i_C = utilities::mod(i_shift_C + Nxc, Nx);
    const t_int j_C = utilities::mod(j_shift_C + Nyc, Ny);
    for(t_uint pix = 0; pix < Grid_.nonZeros(); pix++) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(*(Grid_.outerIndexPtr() + pix), Ny, Nx);
      const t_int i_G = utilities::mod(i_shift_G + Nxc, Nx);
      const t_int j_G = utilities::mod(j_shift_G + Nyc, Ny);
      const t_int i_W = i_G + i_C - Nx;
      const t_int j_W = j_G + j_C - Ny;
      if(-Nxc <= i_W and i_W <= Nxc and -Nyc <= j_W and j_W <= Nyc) {
        const t_int i_shift_W = utilities::mod(i_W, Nx);
        const t_int j_shift_W = utilities::mod(j_W, Ny);
        if(i_shift_C < 0 or j_shift_C < 0)
          throw std::runtime_error("Out of bounds in row convolution.");
        non_zero_W.insert(utilities::sub2ind(j_shift_W, i_shift_W, Ny, Nx));
      }
    }
  }
  if(non_zero_W.size() == 0)
    throw std::runtime_error("Gridding kernel or chirp kernel is zero.");
  Sparse<t_complex> output_row(Nx * Ny, 1);
  output_row.reserve(non_zero_W.size());
  for(t_int k : non_zero_W) {
    // shifting indicies to workout linear convolution
    t_int i_shift_W = 0;
    t_int j_shift_W = 0;
    std::tie(j_shift_W, i_shift_W) = utilities::ind2sub(k, Ny, Nx);
    const t_int i_W = utilities::mod(i_shift_W + Nxc, Nx);
    const t_int j_W = utilities::mod(j_shift_W + Nyc, Ny);
    for(t_uint pix = 0; pix < Grid_.nonZeros(); pix++) {
      t_int i_shift_G = 0;
      t_int j_shift_G = 0;
      std::tie(j_shift_G, i_shift_G) = utilities::ind2sub(*(Grid_.outerIndexPtr() + pix), Ny, Nx);
      const t_int i_G = utilities::mod(i_shift_G + Nxc, Nx);
      const t_int j_G = utilities::mod(j_shift_G + Nyc, Ny);
      const t_int i_C = i_W - i_G;
      const t_int j_C = j_W - j_G;
      // Checking that convolution result is going to be within bounds
      // FFTshift of result to go into gridding matrix
      const t_int i_shift_C = utilities::mod(i_C + Nxc, Nx);
      const t_int j_shift_C = utilities::mod(j_C + Nyc, Ny);
      const t_int pos = utilities::sub2ind(j_shift_C, i_shift_C, Ny, Nx);
      assert(0 <= pos and pos < Nx * Ny);
      output_row.coeffRef(k, 0) += *(Grid_.valuePtr() + pix) * chirp_.coeff(pos, 0);
    }
  }
  return output_row;
}
} // namespace wproj_utilities
} // namespace purify

#endif
