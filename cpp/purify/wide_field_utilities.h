#ifndef PURIFY_WIDE_FIELD_UTILITIES_H
#define PURIFY_WIDE_FIELD_UTILITIES_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#ifdef PURIFY_MPI
#include <sopt/mpi/communicator.h>
#endif
namespace purify {
namespace widefield {
//! estaimte support size of w given u resolution du
t_int w_support(const t_real w, const t_real du, const t_int min, const t_int max);
//! return factors to convert between arcsecond pixel size image space and lambda for uv space
t_real pixel_to_lambda(const t_real cell, const t_uint imsize, const t_real oversample_ratio);
//! converts been a given pixel size dl in the image domain to a pixel size du in the fourier
//! fourier
t_real dl2du(const t_real dl, const t_uint imsize, const t_real oversample_ratio);
//! return cell size from the bandwidth
t_real estimate_cell_size(const t_real max_u, const t_uint imsize, const t_real oversample_ratio);
//! for a given purify cell size in arcsec provide the equivalent miriad cell size in arcsec
t_real equivalent_miriad_cell_size(const t_real cell, const t_uint imsize,
                                   const t_real oversample_ratio);
//! estimate sample desity grid for a given field of view
Matrix<t_complex> estimate_sample_density(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const t_real cellx, const t_real celly,
                                          const t_uint imsizex, const t_uint imsizey,
                                          const t_real oversample_ratio, const t_real scale);
//! create sample density weights for a given field of view, uniform weighting
Vector<t_complex> sample_density_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                                         const t_real cellx, const t_real celly,
                                         const t_uint imsizex, const t_uint imsizey,
                                         const t_real oversample_ratio, const t_real scale);
#ifdef PURIFY_MPI
//! create sample density weights with MPI for a given field of view, uniform weighting with MPI
Vector<t_complex> sample_density_weights(const Vector<t_real> &u, const Vector<t_real> &v,
                                         const t_real cellx, const t_real celly,
                                         const t_uint imsizex, const t_uint imsizey,
                                         const t_real oversample_ratio, const t_real scale,
                                         const sopt::mpi::Communicator &comm);
#endif
//! Work out max L and M directional cosines from image parameters
t_real fov_cosine(t_real const cell, t_uint const imsize);
//! Generate image of DDE for aw-stacking
template <class DDE>
Matrix<t_complex> generate_dde(const DDE &dde, const t_real dl, const t_real dm,
                               const t_uint x_size, const t_uint y_size, const t_real stop_gap) {
  assert(stop_gap <= 1);
  Image<t_complex> output = Image<t_complex>::Zero(y_size, x_size);

  for (t_int l = 0; l < x_size; ++l)
    for (t_int m = 0; m < y_size; ++m) {
      const t_real x = (l - x_size * 0.5) * dl;
      const t_real y = (m - y_size * 0.5) * dm;
      output(m, l) = ((x * x + y * y) < 1 - stop_gap) ? dde(y, x) : 0.;
    }

  return output;
};
//! generates image of chirp and DDE
template <class DDE>
Matrix<t_complex> generate_chirp(const DDE &dde, const t_real w_rate, const t_real dl,
                                 const t_real dm, const t_uint x_size, const t_uint y_size,
                                 const t_real stop_gap = 0.1) {
  const t_real nz = y_size * x_size;
  const t_complex I(0, 1);
  const auto chirp = [=](const t_real y, const t_real x) {
    return dde(y, x) *
           (std::exp(-2 * constant::pi * I * w_rate * (std::sqrt(1 - x * x - y * y) - 1))) /
           std::sqrt(1 - x * x - y * y) / nz;
  };
  return generate_dde(chirp, dl, dm, x_size, y_size, stop_gap);
}
//! Generates image of chirp
Matrix<t_complex> generate_chirp(const t_real w_rate, const t_real dl, const t_real dm,
                                 const t_uint x_size, const t_uint y_size);
}  // namespace widefield
}  // namespace purify
#endif
