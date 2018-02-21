#include "purify/config.h"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <stdio.h>
#include <string>
#include <time.h>
#include <sys/stat.h>
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"

namespace purify {

namespace wproj_utilities {

//! Work out coordinates for w grid from image parameters
Vector<t_real> w_range(const t_real &w_cell, const t_real &w_max);
//! Returns rows needed for w grid given the w degridder
std::vector<t_uint> w_rows(const Sparse<t_complex> &w_degrider);
//! gridding correction for w gridding
t_complex ft_kernel(const t_real &y, const t_real &x, const t_real &n_max, const t_uint &J);
//! Return sparse W plane grid
Sparse<t_complex> w_plane_grid(const t_uint &x_size, const t_uint &y_size, const t_real &cell_x,
                               const t_real &cell_y, const t_real &energy_fraction_chirp,
                               const t_uint &J, const std::vector<t_uint> &w_rows,
                               const t_real &w_cell, const t_real &w_max);
//! Return sparse matrix to degrid W plane
Sparse<t_complex> w_plane_degrid_matrix(const Vector<t_real> &w_range,
                                        const Vector<t_real> &w_components, const t_uint &J);
//! Return expansion of w grid
std::vector<Sparse<t_complex>>
w_expansion(const t_uint &x_size, const t_uint &y_size, const t_real &cell_x, const t_real &cell_y,
            const t_real &energy_fraction_chirp, const std::vector<t_uint> &w_rows,
            const t_real &w_cell, const t_real &w_max,
            const std::vector<std::function<t_complex(t_real, t_real)>> &a_n);
//! Create w_projection matrix using expansion
Sparse<t_complex> w_projection_expansion(const std::vector<Sparse<t_complex>> &plane_expansion,
                                         const Vector<t_real> &w_components,
                                         const Vector<t_real> &w_grid_coords,
                                         const std::vector<t_uint> &w_rows,
                                         const std::vector<std::function<t_complex(t_real)>> &f_w);
//! Return closest grid points, non zero rows of the W grid
const std::vector<t_uint>
w_rows(const Vector<t_real> &w_components, const Vector<t_real> &w_grid_coords);

namespace expansions {
//! Create functions for taylor expansion coefficients and polynomials
std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
taylor(const t_uint order);
//! Create functions for chebyshev expansion coefficients and polynomials
std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
chebyshev(const t_uint order);
} // namespace expansions
} // namespace wproj_utilities

} // namespace purify
