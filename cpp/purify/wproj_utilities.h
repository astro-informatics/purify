#ifndef PURIFY_wproj_utilities_H
#define PURIFY_wproj_utilities_H

#include "purify/config.h"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
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
Sparse<t_complex>
wprojection_matrix(const Sparse<t_complex> &Grid, const t_int &Nx, const t_int &Ny,
                   const Vector<t_real> &w_components, const t_real &cell_x, const t_real &cell_y,
                   const t_real &energy_fraction_chirp, const t_real &energy_fraction_wproj);
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

  const t_int Nx2 = std::floor(Nx * 0.5);
  const t_int Ny2 = std::floor(Ny * 0.5);
  //#pragma omp parallel for
  for(t_int k = 0; k < Nx * Ny; ++k) {
    t_int i = 0;
    t_int j = 0;
    std::tie(i, j) = utilities::ind2sub(k, Nx, Ny);
    t_complex temp(0, 0);
    for(t_uint pix = 0; pix < Grid_.nonZeros(); pix++) {
      t_int ii = 0;
      t_int jj = 0;
      std::tie(ii, jj) = utilities::ind2sub(*(Grid_.innerIndexPtr() + pix), Nx, Ny);

      t_int oldpixi = ii - i;
      if(ii < Nx2)
        oldpixi += Nx; // fftshift
      if((oldpixi >= 0 and oldpixi < Nx)) {
        t_int oldpixj = jj - j;
        if(jj < Ny2)
          oldpixj += Ny; // fftshift
        if((oldpixj >= 0 and oldpixj < Ny)) {
          const t_int pos = oldpixi * Ny + oldpixj;
          if(std::abs(chirp_.coeff(pos, 0)) > 1e-15)
            temp += *(Grid_.valuePtr() + pix) * chirp_.coeff(pos, 0);
        }
      }
    }

    if(std::abs(temp) > 1e-16) {
      t_int iii, jjj;
      if(i < Nx2)
        iii = i + Nx2;
      else {
        iii = i - Nx2;
      }
      if(j < Ny2)
        jjj = j + Ny2;
      else {
        jjj = j - Ny2;
      }
      const t_int pos = utilities::sub2ind(iii, jjj, Nx, Ny);
#pragma omp critical(load1)
      output_row(pos) = temp;
    }
  }
  return output_row.sparseView(1, 1e-12);
}
} // namespace wproj_utilities
} // namespace purify

#endif
