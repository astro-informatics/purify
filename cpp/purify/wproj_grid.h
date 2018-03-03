#ifndef PURIFY_wproj_grid_H
#define PURIFY_wproj_grid_H
#include "purify/config.h"
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <stdio.h>
#include <string>
#include <time.h>
#include <boost/math/special_functions/bessel.hpp>
#include <boost/math/special_functions/chebyshev.hpp>
#include <sys/stat.h>

#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/types.h"

namespace purify {

namespace wproj_utilities {
//! Calculate W Lambert function
template <typename T> t_real w_lambert(T x, const t_real &relative_difference);
//! Work out coordinates for w grid from image parameters
Vector<t_real> w_range(const t_real &w_cell, const t_real &w_max);
//! Return expansion of w grid
std::vector<Sparse<t_complex>>
w_expansion(const t_uint &x_size, const t_uint &y_size, const t_real &cell_x, const t_real &cell_y,
            const t_real &energy_fraction_chirp, const std::vector<t_uint> &w_rows,
            const Vector<t_real> &w_grid_coords,
            const std::vector<std::function<t_complex(t_real, t_real)>> &a_n);
//! Create w_projection matrix using expansion
Sparse<t_complex> w_projection_expansion(const std::vector<Sparse<t_complex>> &plane_expansion,
                                         const Vector<t_real> &w_components,
                                         const Vector<t_real> &w_grid_coords,
                                         const std::vector<t_uint> &w_rows,
                                         const std::vector<std::function<t_complex(t_real)>> &f_w);
//! Create w_projection matrix single row
Sparse<t_complex> w_projection_expansion(const std::vector<Sparse<t_complex>> &plane_expansion,
                                         const t_real &w_component,
                                         const Vector<t_real> &w_grid_coords, const t_uint &w_row,
                                         const std::vector<std::function<t_complex(t_real)>> &f_w);
//! Return closest grid points, non zero rows of the W grid
const std::vector<t_uint>
w_rows(const Vector<t_real> &w_components, const Vector<t_real> &w_grid_coords);

namespace expansions {
//! Create functions for taylor expansion coefficients and polynomials
std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
taylor(const t_uint order, const t_real &cell_x, const t_real &cell_y, const t_uint &x_size,
       const t_uint &y_size);
//! Create functions for chebyshev expansion coefficients and polynomials
std::tuple<std::vector<std::function<t_complex(t_real, t_real)>>,
           std::vector<std::function<t_complex(t_real)>>>
chebyshev(const t_uint order, const t_real &cell_x, const t_real &cell_y, const t_uint &x_size,
          const t_uint &y_size, const t_real &w_cell);
} // namespace expansions
} // namespace wproj_utilities

namespace wproj_utilites {

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

} // namespace wproj_utilites

} // namespace purify
#endif
