#include "purify/operators.h"

namespace purify {

namespace details {
//! Construct gridding matrix
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_complex> &weights, const t_uint &imsizey_,
                                          const t_uint &imsizex_, const t_real &oversample_ratio,
                                          const std::function<t_real(t_real)> kernelu,
                                          const std::function<t_real(t_real)> kernelv,
                                          const t_uint Ju, const t_uint Jv) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  auto omega_to_k = [](const Vector<t_real> &omega) {
    return omega.unaryExpr(std::ptr_fun<double, double>(std::floor));
  };
  const Vector<t_real> k_u = omega_to_k(u - Vector<t_real>::Constant(rows, Ju * 0.5));
  const Vector<t_real> k_v = omega_to_k(v - Vector<t_real>::Constant(rows, Jv * 0.5));

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

  const t_complex I(0, 1);
#pragma omp simd collapse(3)
  for(t_int m = 0; m < rows; ++m) {
    for(t_int ju = 1; ju < std::min(Ju + 1, ftsizeu_ + 1); ++ju) {
      for(t_int jv = 1; jv < std::min(Jv + 1, ftsizev_ + 1); ++jv) {
        const t_uint q = utilities::mod(k_u(m) + ju, ftsizeu_);
        const t_uint p = utilities::mod(k_v(m) + jv, ftsizev_);
        const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        interpolation_matrix.coeffRef(m, index)
            = std::exp(-2 * constant::pi * I * ((k_u(m) + ju) * 0.5 + (k_v(m) + jv) * 0.5))
              * kernelu(u(m) - (k_u(m) + ju)) * kernelv(v(m) - (k_v(m) + jv)) * weights(m);
      }
    }
  }
  return interpolation_matrix;
}
//! Construct gridding matrix with w projection
Sparse<t_complex>
init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
                        const Vector<t_complex> &weights, const t_uint &imsizey_,
                        const t_uint &imsizex_, const t_real oversample_ratio,
                        const std::function<t_real(t_real)> kernelu,
                        const std::function<t_real(t_real)> kernelv, const t_uint Ju,
                        const t_uint Jv, const bool w_term, const t_real &cellx,
                        const t_real &celly, const t_real &energy_chirp_fraction,
                        const t_real &energy_kernel_fraction) {

  const Sparse<t_complex> G = init_gridding_matrix_2d(u, v, weights, imsizey_, imsizex_,
                                                      oversample_ratio, kernelu, kernelv, Ju, Jv);
  if(w_term)
    return wproj_utilities::wprojection_matrix(
        G, std::floor(oversample_ratio * imsizex_), std::floor(oversample_ratio * imsizey_), w,
        cellx, celly, energy_chirp_fraction, energy_kernel_fraction);
  else
    return G;
}

Image<t_real> init_correction2d(const t_real &oversample_ratio, const t_uint &imsizey_,
                                const t_uint imsizex_,
                                const std::function<t_real(t_real)> ftkernelu,
                                const std::function<t_real(t_real)> ftkernelv) {
  /*
     Given the Fourier transform of a gridding kernel, creates the scaling image for gridding
     correction.

*/
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);

  Array<t_real> range;
  range.setLinSpaced(std::max(ftsizeu_, ftsizev_), 0.5, std::max(ftsizeu_, ftsizev_) - 0.5);
  return (1e0 / range.segment(y_start, imsizey_).unaryExpr(ftkernelv)).matrix()
         * (1e0 / range.segment(x_start, imsizex_).unaryExpr(ftkernelu)).matrix().transpose();
}
} // namespace details
} // namespace purify
