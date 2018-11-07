#include "purify/operators.h"
#include "purify/wide_field_utilities.h"
namespace purify {

namespace details {
//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_complex> &weights, const t_uint &imsizey_,
                                          const t_uint &imsizex_, const t_real &oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const std::function<t_real(t_real)> kernelv,
                                          const t_uint Ju /*= 4*/, const t_uint Jv /*= 4*/) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  const t_int jv_max = std::min(Jv, ftsizev_);
  // If I collapse this for loop there is a crash when using MPI... Sparse<>::insert() doesn't work
  // right
#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    for (t_int ju = 1; ju < ju_max + 1; ++ju) {
      for (t_int jv = 1; jv < jv_max + 1; ++jv) {
        const t_real k_u = std::floor(u(m) - ju_max * 0.5);
        const t_real k_v = std::floor(v(m) - jv_max * 0.5);
        const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
        const t_uint p = utilities::mod(k_v + jv, ftsizev_);
        const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        interpolation_matrix.insert(m, index) =
            std::exp(-2 * constant::pi * I * ((k_u + ju) * 0.5 + (k_v + jv) * 0.5)) *
            kernelu(u(m) - (k_u + ju)) * kernelv(v(m) - (k_v + jv)) * weights(m);
      }
    }
  }
  return interpolation_matrix;
}

Image<t_complex> init_correction2d(const t_real &oversample_ratio, const t_uint &imsizey_,
                                   const t_uint &imsizex_,
                                   const std::function<t_real(t_real)> ftkernelu,
                                   const std::function<t_real(t_real)> ftkernelv,
                                   const t_real &w_mean, const t_real &cellx, const t_real &celly) {
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);

  Array<t_real> range;
  range.setLinSpaced(std::max(ftsizeu_, ftsizev_), 0.5, std::max(ftsizeu_, ftsizev_) - 0.5);
  return ((1e0 / range.segment(y_start, imsizey_).unaryExpr(ftkernelv)).matrix() *
          (1e0 / range.segment(x_start, imsizex_).unaryExpr(ftkernelu)).matrix().transpose())
             .array() *
         t_complex(1., 0.) *
        widefield::generate_chirp(w_mean, cellx, celly, imsizex_, imsizey_)
             .array() *
         imsizex_ * imsizey_;
}
}  // namespace details
}  // namespace purify
