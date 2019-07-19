#include "purify/wproj_operators.h"
#include "purify/operators.h"
#include "purify/wide_field_utilities.h"
#include "purify/wkernel_integration.h"

namespace purify {

namespace details {

Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_real> &w, const Vector<t_complex> &weights,
                                          const t_uint imsizey_, const t_uint imsizex_,
                                          const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> &ftkerneluv,
                                          const std::function<t_real(t_real)> &kerneluv,
                                          const t_uint Ju, const t_uint Jw, const t_real cellx,
                                          const t_real celly, const t_real abs_error,
                                          const t_real rel_error, const dde_type dde) {
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_real du = widefield::pixel_to_lambda(cellx, imsizex_, oversample_ratio);
  const t_real dv = widefield::pixel_to_lambda(celly, imsizey_, oversample_ratio);
  PURIFY_HIGH_LOG("du x du: {}, {}", du, dv);
  if (std::abs(du - dv) > 1e-6)
    throw std::runtime_error(
        "Field of view along l and m is not the same, this assumption is required for "
        "w-projection.");
  const t_uint rows = u.size();
  const t_uint cols = ftsizeu_ * ftsizev_;
  if (u.size() != v.size())
    throw std::runtime_error(
        "Size of u and v vectors are not the same for creating gridding matrix.");
  if (u.size() != w.size())
    throw std::runtime_error(
        "Size of u and w vectors are not the same for creating gridding matrix.");
  if (u.size() != weights.size())
    throw std::runtime_error(
        "Size of u and w vectors are not the same for creating gridding matrix.");
  if (Ju > Jw)
    throw std::runtime_error(
        "w kernel size must be at least the size of w=0 kernel, must have Ju <= Jw.");
  // count gridding coefficients with variable support size
  Vector<t_int> total_coeffs = Vector<t_int>::Zero(w.size());
  for (int i = 0; i < w.size(); i++) {
    const t_int Ju_max =
        widefield::w_support(w(i), du, static_cast<t_int>(Ju), static_cast<t_int>(Jw));
    total_coeffs(i) = Ju_max * Ju_max;
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

  const t_complex I(0., 1.);

  const t_uint max_evaluations = 1e8;
  const t_real relative_error = rel_error;
  const t_real absolute_error = abs_error;

  auto const ftkernel_radial = [&](const t_real l) -> t_real { return ftkerneluv(l); };

  t_int coeffs_done = 0;
  t_uint total = 0;

#pragma omp parallel for
  for (t_int m = 0; m < rows; ++m) {
    // w_projection convolution setup
    const t_int Ju_max = widefield::w_support(w(m), du, Ju, Jw);
    t_uint evaluations = 0;
    const t_int kwu = std::floor(u(m) - Ju_max * 0.5);
    const t_int kwv = std::floor(v(m) - Ju_max * 0.5);
    const t_real w_val = w(m);  //((0. < w(m)) ? 1: -1) * std::min(Ju_max * du, std::abs(w(m)));

    for (t_int ju = 1; ju < Ju_max + 1; ++ju) {
      for (t_int jv = 1; jv < Ju_max + 1; ++jv) {
        const t_uint q = utilities::mod(kwu + ju, ftsizeu_);
        const t_uint p = utilities::mod(kwv + jv, ftsizev_);
        const t_uint index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
        interpolation_matrix.insert(m, index) =
            std::exp(-2 * constant::pi * I * ((kwu + ju) * 0.5 + (kwv + jv) * 0.5)) * weights(m) *
            ((dde == dde_type::wkernel_radial)
                 ? ((std::abs(2 * constant::pi * w_val * (std::sqrt(1 - std::pow(0.5 / du, 2))) -
                              1) > 0.01)
                        ? projection_kernels::exact_w_projection_integration_1d(
                              (u(m) - (kwu + ju)), (v(m) - (kwv + jv)), w_val, du, oversample_ratio,
                              ftkernel_radial, max_evaluations, absolute_error, relative_error,
                              (du > 1.) ? integration::method::p : integration::method::h,
                              evaluations)
                        : kerneluv(std::sqrt(std::pow(u(m) - (kwu + ju), 2) +
                                             std::pow(v(m) - (kwv + jv), 2))))
                 : projection_kernels::exact_w_projection_integration(
                       (u(m) - (kwu + ju)), (v(m) - (kwv + jv)), w_val, du, dv, oversample_ratio,
                       ftkernel_radial, ftkernel_radial, max_evaluations, absolute_error,
                       relative_error, integration::method::h, evaluations));
#pragma omp critical(add_eval)
        {
          coeffs_done++;
          if (num_of_coeffs > 100) {
            if ((coeffs_done % (static_cast<t_int>(num_of_coeffs / 100)) == 0)) {
              PURIFY_LOW_LOG(
                  "w = {}, support = {}x{}, coeffs: {} of {}, {}%", w_val, Ju_max, Ju_max,
                  coeffs_done, num_of_coeffs,
                  static_cast<t_real>(coeffs_done) / static_cast<t_real>(num_of_coeffs) * 100.);
            }
          }
        }
      }
    }
  }
  interpolation_matrix.makeCompressed();
  return interpolation_matrix;
}

Image<t_complex> init_correction_radial_2d(const t_real oversample_ratio, const t_uint imsizey_,
                                           const t_uint imsizex_,
                                           const std::function<t_real(t_real)> &ftkerneluv,
                                           const t_real w_mean, const t_real cellx,
                                           const t_real celly) {
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);

  Image<t_complex> gridding_correction = Image<t_complex>::Zero(imsizey_, imsizex_);
  for (int i = 0; i < gridding_correction.rows(); i++)
    for (int j = 0; j < gridding_correction.cols(); j++)
      gridding_correction(i, j) =
          1. / ftkerneluv(std::sqrt(std::pow((y_start + i + 0.5) / ftsizev_ - 0.5, 2) +
                                    std::pow((j + 0.5 + x_start) / ftsizeu_ - 0.5, 2)));

  return gridding_correction.array() *
         widefield::generate_chirp(w_mean, cellx, celly, imsizex_, imsizey_).array() * imsizex_ *
         imsizey_;
}

}  // namespace details

}  // namespace purify
