#include "purify/wide_field_utilities.h"
#include "purify/utilities.h"

namespace purify {
namespace widefield {

t_int w_support(const t_real w, const t_real du, const t_int min, const t_int max) {
  return std::min<int>(std::max<int>(static_cast<int>(2 * std::floor(std::abs(w / du))), min), max);
}

t_real pixel_to_lambda(const t_real cell, const t_uint imsize, const t_real oversample_ratio) {
  return 1. / (oversample_ratio * fov_cosine(cell, imsize));
}

t_real estimate_cell_size(const t_real max_u, const t_uint imsize, const t_real oversample_ratio) {
  return (2. / static_cast<t_real>(imsize)) *
         std::asin(static_cast<t_real>(imsize) / (4 * oversample_ratio * max_u)) * 60. * 60. *
         180. / constant::pi;
}

t_real fov_cosine(t_real const cell, t_uint const imsize) {
  const t_real theta_FoV_L = imsize * cell;
  const t_real extra_theta = (theta_FoV_L > 180. * 60. * 60.) ? theta_FoV_L - 180. * 60. * 60. : 0.;
  return 2 * std::sin(constant::pi / 180. * (theta_FoV_L - extra_theta) / (60. * 60.) * 0.5) +
         2 * std::sin(constant::pi / 180. * extra_theta / (60. * 60.) * 0.5);
}

Matrix<t_complex> generate_chirp(const t_real w_rate, const t_real cell_x, const t_real cell_y,
                                 const t_uint x_size, const t_uint y_size) {
  return generate_chirp([](t_real, t_real) { return 1.; }, w_rate, cell_x, cell_y, x_size, y_size);
}

Matrix<t_complex> estimate_sample_density(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const t_real cellx, const t_real celly,
                                          const t_uint imsizex, const t_uint imsizey,
                                          const t_real oversample_ratio, const t_real scale) {
  const t_int ftsizeu = std::floor(oversample_ratio * imsizex);
  const t_int ftsizev = std::floor(oversample_ratio * imsizex);
  Matrix<t_complex> sample_density = Matrix<t_complex>::Zero(ftsizev, ftsizeu);
  const t_real du = pixel_to_lambda(cellx, imsizex, oversample_ratio) * scale;
  const t_real dv = pixel_to_lambda(celly, imsizey, oversample_ratio) * scale;
  for (t_int i = 0; i < u.size(); ++i) {
    const t_int q = utilities::mod(floor(u(i) * du), ftsizeu);
    const t_int p = utilities::mod(floor(v(i) * dv), ftsizev);
    sample_density(p, q) += 1.;
  }
  return sample_density;
}

Vector<t_complex> sample_density_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                                         const t_real cellx, const t_real celly,
                                         const t_uint imsizex, const t_uint imsizey,
                                         const t_real oversample_ratio, const t_real scale) {
  Vector<t_complex> weights = Vector<t_complex>::Zero(u.size());
  const t_int ftsizeu = std::floor(oversample_ratio * imsizex);
  const t_int ftsizev = std::floor(oversample_ratio * imsizex);
  const Matrix<t_complex> sample_density =
      estimate_sample_density(u, v, cellx, celly, imsizex, imsizey, oversample_ratio, scale);
  const t_real du = pixel_to_lambda(cellx, imsizex, oversample_ratio) * scale;
  const t_real dv = pixel_to_lambda(celly, imsizey, oversample_ratio) * scale;
  for (t_int i = 0; i < u.size(); ++i) {
    const t_int q = utilities::mod(floor(u(i) * du), ftsizeu);
    const t_int p = utilities::mod(floor(v(i) * dv), ftsizev);
    weights(i) = 1. / sample_density(p, q);
  }
  const t_real max_weight = weights.array().cwiseAbs().maxCoeff();
  weights /= max_weight;
  return weights;
}
#ifdef PURIFY_MPI
Vector<t_complex> sample_density_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                                         const t_real cellx, const t_real celly,
                                         const t_uint imsizex, const t_uint imsizey,
                                         const t_real oversample_ratio,
                                         const t_real scale, const sopt::mpi::Communicator &comm) {
  Vector<t_complex> weights = Vector<t_complex>::Zero(u.size());
  const t_int ftsizeu = std::floor(oversample_ratio * imsizex);
  const t_int ftsizev = std::floor(oversample_ratio * imsizex);
  const Matrix<t_complex> sample_density = comm.all_sum_all(
      estimate_sample_density(u, v, cellx, celly, imsizex, imsizey, oversample_ratio, scale));
  const t_real du = pixel_to_lambda(cellx, imsizex, oversample_ratio) * scale;
  const t_real dv = pixel_to_lambda(celly, imsizey, oversample_ratio) * scale;
  for (t_int i = 0; i < u.size(); ++i) {
    const t_int q = utilities::mod(floor(u(i) * du), ftsizeu);
    const t_int p = utilities::mod(floor(v(i) * dv), ftsizev);
    weights(i) = 1. / sample_density(p, q);
  }
  const t_real max_weight = comm.all_reduce<t_real>(weights.array().cwiseAbs().maxCoeff(), MPI_MAX);
  weights /= max_weight;
  return weights;
}
#endif
}  // namespace widefield
}  // namespace purify
