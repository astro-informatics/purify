#include "purify/operators.h"

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
  auto omega_to_k = [](const Vector<t_real> &omega) {
    return omega.unaryExpr(std::ptr_fun<double, double>(std::floor));
  };
  const Vector<t_real> k_u = omega_to_k(u - Vector<t_real>::Constant(rows, Ju * 0.5));
  const Vector<t_real> k_v = omega_to_k(v - Vector<t_real>::Constant(rows, Jv * 0.5));
  if(u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

  const t_complex I(0, 1);
  const t_int ju_max = std::min(Ju + 1, ftsizeu_ + 1);
  const t_int jv_max = std::min(Jv + 1, ftsizev_ + 1);
#pragma omp parallel for collapse(3)
  for(t_int m = 0; m < rows; ++m) {
    for(t_int ju = 1; ju < ju_max; ++ju) {
      for(t_int jv = 1; jv < jv_max; ++jv) {
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
Sparse<t_complex> init_gridding_matrix_2d(
    const Vector<t_real> &u, const Vector<t_real> &v, const Vector<t_real> &w,
    const Vector<t_complex> &weights, const t_uint &imsizey_, const t_uint &imsizex_,
    const t_real oversample_ratio, const std::function<t_real(t_real)> kernelu,
    const std::function<t_real(t_real)> kernelv, const t_uint Ju /*= 4*/, const t_uint Jv /*= 4*/,
    const bool w_term /*= false*/, const t_real &cellx /*= 1*/, const t_real &celly /*= 1*/,
    const t_real &energy_chirp_fraction /*= 1*/, const t_real &energy_kernel_fraction /*= 1*/,
    const wproj_utilities::expansions::series
        series /*= wproj_utilities::expansions::series::none */,
    const t_uint order /* = 1 */, const t_real &interpolation_error /*= 1e-2 */) {

  const Sparse<t_complex> G = init_gridding_matrix_2d(u, v, weights, imsizey_, imsizex_,
                                                      oversample_ratio, kernelu, kernelv, Ju, Jv);
  if(w_term)
    return wproj_utilities::wprojection_matrix(G, std::floor(oversample_ratio * imsizex_),
                                               std::floor(oversample_ratio * imsizey_), w, cellx,
                                               celly, energy_chirp_fraction, energy_kernel_fraction,
                                               series, order, interpolation_error);
  else
    return G;
}

Image<t_real> init_correction2d(const t_real &oversample_ratio, const t_uint &imsizey_,
                                const t_uint imsizex_,
                                const std::function<t_real(t_real)> ftkernelu,
                                const std::function<t_real(t_real)> ftkernelv) {

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
