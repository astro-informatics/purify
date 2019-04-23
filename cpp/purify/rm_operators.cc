#include "purify/rm_operators.h"
#include "purify/rm_kernel_integration.h"
namespace purify {

namespace rm_details {
t_real pixel_to_lambda2(const t_real cell, const t_uint imsize, const t_real oversample_ratio) {
  return constant::pi / (oversample_ratio * cell * imsize);
}
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
      const t_uint index = utilities::mod(k_u + ju, ftsizeu_);
      interpolation_matrix.insert(m, index) = std::exp(-2 * constant::pi * I * (k_u + ju) * 0.5) *
                                              kernelu(u(m) - (k_u + ju)) * weights(m);
    }
  }
  return interpolation_matrix;
}
Sparse<t_complex> init_gridding_matrix_1d(const Vector<t_real> &u, const Vector<t_real> &widths,
                                          const Vector<t_complex> &weights, const t_uint imsizex_,
                                          const t_real cell, const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> ftkernelu,
                                          const t_uint Ju, const t_uint J_max,
                                          const t_real rel_error, const t_real abs_error) {
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_int rows = u.size();
  const t_int cols = ftsizeu_;

  const t_real du = rm_details::pixel_to_lambda2(cell, imsizex_, oversample_ratio);

  // count gridding coefficients with variable support size
  Vector<t_int> total_coeffs = Vector<t_int>::Zero(widths.size());
  for (int i = 0; i < widths.size(); i++) {
    const t_int Ju_max = std::min<t_int>(std::floor(Ju + widths(i) / du), J_max);
    total_coeffs(i) = Ju_max;
  }
  const t_real num_of_coeffs = total_coeffs.array().cast<t_real>().sum();
  PURIFY_HIGH_LOG("Using {} rows (coefficients per a row {}), and memory of {} MegaBytes",
                  total_coeffs.size(), total_coeffs.array().cast<t_real>().mean(),
                  16. * num_of_coeffs / std::pow(10., 6));
  Sparse<t_complex> interpolation_matrix(rows, cols);
  try {
    interpolation_matrix.reserve(total_coeffs);
  } catch (std::bad_alloc e) {
    throw std::runtime_error(
        "Not enough memory for coefficients, choose upper limit on support size Jw.");
  }
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju));

  const t_complex I(0, 1);
  const t_uint max_evaluations = 1e9;
  // If I collapse this for loop there is a crash when using MPI... Sparse<>::insert() doesn't work
  // right
#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    const t_int ju_max = std::min<t_int>(std::floor(Ju + widths(m) / du), J_max);
    for (t_int ju = 1; ju < ju_max + 1; ++ju) {
      const t_real k_u = std::floor(u(m) - ju_max * 0.5);
      const t_uint index = utilities::mod(k_u + ju, ftsizeu_);
      t_uint evaluations = 0;
      interpolation_matrix.insert(m, index) =
          std::exp(-2 * constant::pi * I * (k_u + ju) * 0.5) *
          projection_kernels::exact_rm_projection_integration(
              u(m) - (k_u + ju), widths(m), du, oversample_ratio, ftkernelu, max_evaluations,
              abs_error, rel_error, integration::method::p, evaluations) *
          weights(m);
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
}  // namespace rm_details
}  // namespace purify
