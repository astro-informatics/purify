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
#include "purify/MeasurementOperator.h"
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
t_real sparsify_row_thres(const Sparse<t_real> &row, const t_real &energy);
//! Returns threshold to keep a fraction of energy in the dense row
t_real sparsify_row_dense_thres(const Matrix<t_complex> &row, const t_real &energy);
//! Perform convolution with gridding matrix row and chirp matrix row
Sparse<t_complex> row_wise_convolution(Eigen::SparseVector<t_complex> &Grid,
                                       Sparse<t_complex> &Chirp, const t_int &Nx, const t_int &Ny);
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

} // namespace wproj_utilities
} // namespace purify

#endif
