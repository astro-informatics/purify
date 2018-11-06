#ifndef PURIFY_WPROJ_OPERATORS_H
#define PURIFY_WPROJ_OPERATORS_H

#include "purify/config.h"
#include "purify/types.h"

#include "operators.h"

namespace purify {

namespace details {

//! Construct gridding matrix with wprojection
Sparse<t_complex> init_gridding_matrix_2d(const Vector<t_real> &u, const Vector<t_real> &v,
                                          const Vector<t_real> &w, const Vector<t_complex> &weights,
                                          const t_uint imsizey_, const t_uint imsizex_,
                                          const t_real oversample_ratio,
                                          const std::function<t_real(t_real)> &ftkerneluv,
                                          const t_uint Ju, const t_uint Jw, const t_real cellx,
                                          const t_real celly, const t_real abs_error,
                                          const t_real rel_error);

//! Given the Fourier transform of a radially symmetric gridding kernel, creates the scaling image
//! for gridding correction.
Image<t_complex> init_correction_radial_2d(const t_real oversample_ratio, const t_uint imsizey_,
                                           const t_uint imsizex_,
                                           const std::function<t_real(t_real)> &ftkerneluv,
                                           const t_real w_mean, const t_real cellx,
                                           const t_real celly);

}  // namespace details

namespace operators {

template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_padding_and_FFT_2d(
    const std::function<t_real(t_real)> &ftkerneluv, const t_uint imsizey, const t_uint imsizex,
    const t_real oversample_ratio,const fftw_plan ft_plan, const t_real w_mean, const t_real cellx, const t_real celly) {
  sopt::OperatorFunction<T> directZ, indirectZ;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  const Image<t_complex> S = purify::details::init_correction_radial_2d(
      oversample_ratio, imsizey, imsizex, ftkerneluv, w_mean, cellx, celly);
  PURIFY_LOW_LOG("Building Measurement Operator: WGFZDB");
  PURIFY_LOW_LOG(
      "Constructing Zero Padding "
      "and Correction Operator: "
      "ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor: {}", oversample_ratio);
  std::tie(directZ, indirectZ) = purify::operators::init_zero_padding_2d<T>(S, oversample_ratio);
  PURIFY_LOW_LOG("Constructing FFT operator: F");
  switch (ft_plan) {
  case fftw_plan::measure:
    PURIFY_MEDIUM_LOG("Measuring Plans...");
    break;
  case fftw_plan::estimate:
    PURIFY_MEDIUM_LOG("Estimating Plans...");
    break;
  }
  std::tie(directFFT, indirectFFT) =
      purify::operators::init_FFT_2d<T>(imsizey, imsizex, oversample_ratio, ft_plan);
  auto direct = sopt::chained_operators<T>(directFFT, directZ);
  auto indirect = sopt::chained_operators<T>(indirectZ, indirectFFT);
  return std::make_tuple(direct, indirect);
}

}  // namespace operators

}  // namespace purify
#endif
