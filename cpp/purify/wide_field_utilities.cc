#include "wide_field_utilities.h"

namespace purify {
namespace widefield {

t_int w_support(const t_real w, const t_real du, const t_int min, const t_int max) {
  return std::min<int>(std::max<int>(static_cast<int>(2 * std::floor(std::abs(w / du))), min), max);
}

t_real pixel_to_lambda(const t_real cell, const t_uint imsize, const t_real oversample_ratio) {
  return 1. / ( oversample_ratio * fov_cosine(cell, imsize));
}

t_real estimate_cell_size(const t_real max_u, const t_uint imsize, const t_real oversample_ratio) {
  return (2. / static_cast<t_real>(imsize)) *
         std::asin(static_cast<t_real>(imsize) / (4 * oversample_ratio * max_u)) * 60. * 60. *
         180. / constant::pi;
}

t_real fov_cosine(t_real const cell, t_uint const imsize) {
  const t_real theta_FoV_L = imsize * cell;
  return 2 * std::sin(constant::pi / 180. * theta_FoV_L / (60. * 60.) * 0.5);
}

Matrix<t_complex> generate_chirp(const t_real w_rate, const t_real cell_x, const t_real cell_y,
                                 const t_uint x_size, const t_uint y_size) {
  return generate_chirp([](t_real, t_real) { return 1.; }, w_rate, cell_x, cell_y, x_size, y_size);
}
}  // namespace widefield
}  // namespace purify
