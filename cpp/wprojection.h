#ifndef PURIFY_WPROJECTION_H
#define PURIFY_WPROJECTION_H

#include "types.h"

#include <string>
#include "FFTOperator.h"
#include "utilities.h"

namespace purify {

namespace wprojection {
//! Calculates upsample ratio for w-projection
t_real upsample_ratio(const utilities::vis_params &uv_vis, const t_real &cell_x,
                      const t_real &cell_y, const t_int &x_size, const t_int &y_size);
//! Calculates convolution between grid and chirp matrix
Sparse<t_complex> convolution(const Sparse<t_complex> &Grid, const Image<t_complex> &Chirp,
                              const t_int &Nx, const t_int &Ny, const t_int &Nvis);
//! Sparsifies chirp
Image<t_complex> sparsify_chirp(const Image<t_complex> &row, const t_real &energy);
//! Generates image of chirp for w component
Image<t_complex> generate_chirp(const t_real w_term, const t_real cellx, const t_real celly,
                                const t_int x_size, const t_int y_size);
//! Generates chirp matrix
Matrix<t_complex> create_chirp_matrix(const Vector<t_real> &w_components, const t_real cell_x,
                                      const t_real cell_y, const t_int &ftsizeu,
                                      const t_int &ftsizev, const t_real &energy_fraction = 1);
}
}

#endif