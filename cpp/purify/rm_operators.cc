#include "purify/rm_operators.h"
namespace purify {

namespace details {
//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_1d(const Vector<t_real> &u, const Vector<t_complex> &weights,
                                          const t_uint imsizex_, const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const t_uint Ju) {
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_;

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju));

  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju, ftsizeu_);
  // If I collapse this for loop there is a crash when using MPI... Sparse<>::insert() doesn't work
  // right
#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    for (t_int ju = 1; ju < ju_max + 1; ++ju) {
      const t_real k_u = std::floor(u(m) - ju_max * 0.5);
      const t_uint q = utilities::mod(k_u + ju, ftsizeu_);
      const t_uint index = utilities::sub2ind(0, q, 1, ftsizeu_);
      interpolation_matrix.insert(m, index) = std::exp(-2 * constant::pi * I * (k_u + ju) * 0.5) *
                                              kernelu(u(m) - (k_u + ju)) * weights(m);
    }
  }
  return interpolation_matrix;
}

Image<t_complex> init_correction1d(const t_real oversample_ratio, const t_uint imsizex_,
                                   const std::function<t_real(t_real)> ftkernelu) {
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);

  Array<t_real> range;
  range.setLinSpaced(ftsizeu_, 0.5, ftsizeu_ - 0.5);
  return (1e0 / range.segment(x_start, imsizex_).unaryExpr([&](t_real x) {
           return ftkernelu(x / static_cast<t_real>(ftsizeu_) - 0.5);
         })) *
         t_complex(1., 0.);
}
}  // namespace details
}  // namespace purify
