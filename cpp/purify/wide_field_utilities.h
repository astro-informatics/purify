#ifndef PURIFY_WIDE_FIELD_UTILITIES_H
#define PURIFY_WIDE_FIELD_UTILITIES_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
namespace purify {
namespace widefield {
//! estaimte support size of w given u resolution du
t_int w_support(const t_real w, const t_real du, const t_int min, const t_int max);
//! return factors to convert between arcsecond pixel size image space and lambda for uv space
t_real pixel_to_lambda(const t_real cell, const t_uint imsize, const t_real oversample_ratio);
//! Work out max L and M directional cosines from image parameters
t_real fov_cosine(t_real const cell, t_uint const imsize);
//! Generate image of DDE for aw-stacking
template <class DDE>
Matrix<t_complex> generate_dde(const DDE &dde,
                               const t_real cell_x, const t_real cell_y, const t_uint x_size,
                               const t_uint y_size){

  const t_real L = fov_cosine(cell_x, x_size);
  const t_real M = fov_cosine(cell_y, y_size);

  const t_real delt_x = L / x_size;
  const t_real delt_y = M / y_size;
  Image<t_complex> output = Image<t_complex>::Zero(y_size, x_size);

  for (t_int l = 0; l < x_size; ++l)
    for (t_int m = 0; m < y_size; ++m) {
      const t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
      const t_real y = (m + 0.5 - y_size * 0.5) * delt_y;
      output(l, m) = dde(y, x);
    }

  return output;

};
//! generates image of chirp and DDE
template <class DDE>
Matrix<t_complex> generate_chirp(const DDE &dde,
                                 const t_real w_rate, const t_real cell_x, const t_real cell_y,
                                 const t_uint x_size, const t_uint y_size) {
  const t_real nz = y_size * x_size;
  const t_complex I(0, 1);
  const auto chirp = [=](const t_real y, const t_real x) {
    return dde(y, x) *
           (std::exp(-2 * constant::pi * I * w_rate * (std::sqrt(1 - x * x - y * y) - 1))) / std::sqrt(1 - x * x - y * y)/ nz;
  };
  return generate_dde(chirp, cell_x, cell_y, x_size, y_size);
}
//! Generates image of chirp
Matrix<t_complex> generate_chirp(const t_real w_rate, const t_real cell_x, const t_real cell_y,
                                 const t_uint x_size, const t_uint y_size);
}  // namespace widefield
}  // namespace purify
#endif
