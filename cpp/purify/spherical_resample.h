#ifndef PURIFY_SPHERICAL_RESAMPLE_H
#define PURIFY_SPHERICAL_RESAMPLE_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/operators.h"

namespace purify {
namespace spherical_resample {
//! calculate l directional cosine for a given pointing given angles
t_real calculate_l(const t_real theta, const t_real phi);
//! calculate m directional cosine for a given pointing given angles
t_real calculate_m(const t_real theta, const t_real phi);
//! calculate n directional cosine for a given pointing given angles
t_real calculate_n(const t_real phi);
//! calculate the rotated l from euler angles in zyz and starting coordinates (l, m, n)
t_real calculate_rotated_l(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma);
//! calculate the rotated m from euler angles in zyz and starting coordinates (l, m, n)
t_real calculate_rotated_m(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma);
//! calculate the rotated n from euler angles in zyz and starting coordinates (l, m, n)
t_real calculate_rotated_n(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma);
//! calculate the l in a rotated frame from euler angles in zyz
t_real calculate_l(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma);
//! calculate the m in a rotated frame from euler angles in zyz
t_real calculate_m(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma);
//! calculate the n in a rotated frame from euler angles in zyz
t_real calculate_n(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma);

//! generate indicies that overlap with imaging field of view for resampling
std::vector<t_int> generate_indicies(const Vector<t_real> &l, const Vector<t_real> &m,
                                     const Vector<t_real> &n, const t_int imsizey,
                                     const t_int imsizex, const t_real dl, const t_real dm);
//! generate a mask for imaged field of view that is going to be resampled
Vector<t_real> generate_mask(const Vector<t_real> &l, const Vector<t_real> &m,
                             const Vector<t_real> &n, const t_int imsizey, const t_int imsizex,
                             const t_real dl, const t_real dm);
//! sparse matrix that degrids/grids between spherical sampling pattern and regular grid
Sparse<t_complex> init_resample_matrix_2d(const Vector<t_real> &l, const Vector<t_real> &m,
                                          const t_int imsizey, const t_int imsizex,
                                          const std::function<t_real(t_real)> kernell,
                                          const std::function<t_real(t_real)> kernelm,
                                          const t_int Jl, const t_int Jm);

//! generate the l and m coordinates in pixels and their indicies for resampling
template <class T>
std::tuple<Vector<t_real>, Vector<t_real>, std::vector<t_int>> calculate_compressed_lm(
    const t_int number_of_samples, const t_real theta_0, const t_real phi_0, const T &theta,
    const T &phi, const t_int imsizey, const t_int imsizex, const t_real dl, const t_real dm) {
  Vector<t_real> l = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> m = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> n = Vector<t_real>::Zero(number_of_samples);
  for (t_int k = 0; k < number_of_samples; k++) {
    l(k) = calculate_l(theta(k), phi(k), 0., phi_0, theta_0);
    m(k) = calculate_m(theta(k), phi(k), 0., phi_0, theta_0);
    n(k) = calculate_n(theta(k), phi(k), 0., phi_0, theta_0);
  }
  const std::vector<t_int> indicies = generate_indicies(l, m, n, imsizey, imsizex, dl, dm);
  if (indicies.size() < 1)
    throw std::runtime_error(
        "indicies is zero when constructing spherical resampling; check number_of_samples.");
  Vector<t_real> l_compressed = Vector<t_real>::Zero(indicies.size());
  Vector<t_real> m_compressed = Vector<t_real>::Zero(indicies.size());
  for (t_int k = 0; k < indicies.size(); k++) {
    l_compressed(k) = l(indicies.at(k)) / dl;
    m_compressed(k) = m(indicies.at(k)) / dm;
  }
  return std::make_tuple(l_compressed, m_compressed, indicies);
}
//! cutout region of sphere used for resampling
template <class K>
std::tuple<sopt::OperatorFunction<K>, sopt::OperatorFunction<K>> init_pop_indicies_operator(
    const std::vector<t_int> &indicies, const t_int number_of_samples) {
  return std::make_tuple(
      [=](K &output, const K &input) {
        assert(input.size() == number_of_samples);
        output = K::Zero(indicies.size());
        for (t_int k = 0; k < indicies.size(); k++) output(k) = input(indicies.at(k));
      },
      [=](K &output, const K &input) {
        assert(input.size() == indicies.size());
        output = K::Zero(number_of_samples);
        for (t_int k = 0; k < indicies.size(); k++) output(indicies.at(k)) = input(k);
      });
}

//! returns an operator that will resample from a subset of coordinates
template <class K>
std::tuple<sopt::OperatorFunction<K>, sopt::OperatorFunction<K>> init_resample_operator_2d(
    const t_int number_of_samples, const Vector<t_real> &l_compressed,
    const Vector<t_real> &m_compressed, const std::vector<t_int> &indicies, const t_int imsizey,
    const t_int imsizex, const t_real dl, const t_real dm,
    const std::function<t_real(t_real)> &kernell, const std::function<t_real(t_real)> &kernelm,
    const t_int Jl, const t_int Jm) {
  if ((Jl <= 0) or (Jm <= 0))
    throw std::runtime_error("Support size of resampling kernel is not positive");
  if ((imsizex <= 0) or (imsizey <= 0))
    throw std::runtime_error("Tangent image size is not greater than zero.");
  if (number_of_samples <= 0)
    throw std::runtime_error("Number of samples on the sphere is not greater than zero.");
  if (indicies.size() != l_compressed.size())
    throw std::runtime_error("Indicies is not the same size as l_compressed.");
  if (indicies.size() != l_compressed.size())
    throw std::runtime_error("Indicies is not the same size as m_compressed.");

  const Sparse<t_complex> interrpolation_matrix = init_resample_matrix_2d(
      l_compressed, m_compressed, imsizey, imsizex, kernell, kernelm, Jl, Jm);
  const Sparse<t_complex> interrpolation_matrix_adjoint = interrpolation_matrix.adjoint();
  return std::make_tuple(
      [=](K &output, const K &input) { output = interrpolation_matrix_adjoint * input; },
      [=](K &output, const K &input) {
        assert(input.size() == imsizex * imsizey);
        output = interrpolation_matrix * input;
      });
}

//! return operator that will resample between the sphere and the plane with masking
template <class K, class T>
std::tuple<sopt::OperatorFunction<K>, sopt::OperatorFunction<K>> init_mask_and_resample_operator_2d(
    const t_int number_of_samples, const t_real theta_0, const t_real phi_0, const T &theta,
    const T &phi, const t_int imsizey, const t_int imsizex, const t_real dl, const t_real dm,
    const std::function<t_real(t_real)> &kernell, const std::function<t_real(t_real)> &kernelm,
    const t_int Jl, const t_int Jm) {
  if ((Jl <= 0) or (Jm <= 0))
    throw std::runtime_error("Support size of resampling kernel is not positive");
  if ((imsizex <= 0) or (imsizey <= 0))
    throw std::runtime_error("Tangent image size is not greater than zero.");
  if (number_of_samples <= 0)
    throw std::runtime_error("Number of samples on the sphere is not greater than zero.");
  // Find indicies for cutting to resample
  const auto lm_with_mask = calculate_compressed_lm<T>(number_of_samples, theta_0, phi_0, theta,
                                                       phi, imsizey, imsizex, dl, dm);
  const Vector<t_real> &l_compressed = std::get<0>(lm_with_mask);
  const Vector<t_real> &m_compressed = std::get<1>(lm_with_mask);
  const std::vector<t_int> &indicies = std::get<2>(lm_with_mask);
  // Create operator to cutout region of sphere for resampling
  const auto cutting_operator = init_pop_indicies_operator<K>(indicies, number_of_samples);
  // Operator to perform resampling
  const auto gridding_operator =
      init_resample_operator_2d<K>(number_of_samples, l_compressed, m_compressed, indicies, imsizey,
                                   imsizex, dl, dm, kernell, kernelm, Jl, Jm);
  // Combining them together
  auto direct =
      sopt::chained_operators<K>(std::get<0>(gridding_operator), std::get<0>(cutting_operator));
  auto indirect =
      sopt::chained_operators<K>(std::get<1>(cutting_operator), std::get<1>(gridding_operator));
  return std::make_tuple(direct, indirect);
}

//! gridding correction and zero padding in the FT domain (with FFT shift operations accounted for)
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> init_FT_zero_padding_2d(
    const Image<typename T::Scalar> &S, const t_real oversample_ratio) {
  const t_uint imsizex_ = S.cols();
  const t_uint imsizey_ = S.rows();
  const t_uint ftsizeu_ = std::floor(imsizex_ * oversample_ratio);
  const t_uint ftsizev_ = std::floor(imsizey_ * oversample_ratio);
  const t_uint x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
  const t_uint y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
  auto direct = [=](T &output, const T &x) {
    assert(x.size() == ftsizeu_ * ftsizev_);
    output = T::Zero(imsizey_ * imsizex_);
    for (t_uint j = 0; j < imsizey_; j++) {
      for (t_uint i = 0; i < imsizex_; i++) {
        const t_uint output_index =
            utilities::sub2ind(utilities::mod(j - imsizey_ * 0.5, imsizey_),
                               utilities::mod(i - imsizex_ * 0.5, imsizex_), imsizey_, imsizex_);
        const t_uint input_index = utilities::sub2ind(
            utilities::mod(y_start + j - ftsizev_ * 0.5, ftsizev_),
            utilities::mod(x_start + i - ftsizeu_ * 0.5, ftsizeu_), ftsizev_, ftsizeu_);
        output(output_index) = S(j, i) * x(input_index);
      }
    }
  };
  auto indirect = [=](T &output, const T &x) {
    assert(x.size() == imsizex_ * imsizey_);
    output = T::Zero(ftsizeu_ * ftsizev_);
    for (t_uint j = 0; j < imsizey_; j++) {
      for (t_uint i = 0; i < imsizex_; i++) {
        const t_uint input_index =
            utilities::sub2ind(utilities::mod(j - imsizey_ * 0.5, imsizey_),
                               utilities::mod(i - imsizex_ * 0.5, imsizex_), imsizey_, imsizex_);
        const t_uint output_index = utilities::sub2ind(
            utilities::mod(y_start + j - ftsizev_ * 0.5, ftsizev_),
            utilities::mod(x_start + i - ftsizeu_ * 0.5, ftsizeu_), ftsizev_, ftsizeu_);
        output(output_index) = std::conj(S(j, i)) * x(input_index);
      }
    }
  };
  return std::make_tuple(direct, indirect);
}

//! generates operator to scale, zero padd, FFT, crop, scale
template <class T>
std::tuple<sopt::OperatorFunction<T>, sopt::OperatorFunction<T>> base_padding_and_FFT_2d(
    const std::function<t_real(t_real)> &ftkernelu, const std::function<t_real(t_real)> &ftkernelv,
    const std::function<t_real(t_real)> &ftkernell, const std::function<t_real(t_real)> &ftkernelm,
    const t_uint imsizey, const t_uint imsizex, const t_real oversample_ratio = 2,
    const t_real oversample_ratio_image_domain = 2,
    const operators::fftw_plan &ft_plan = operators::fftw_plan::measure, const t_real w_mean = 0,
    const t_real cellx = 1, const t_real celly = 1) {
  sopt::OperatorFunction<T> directZ_image_domain, indirectZ_image_domain;
  sopt::OperatorFunction<T> directZ_ft_domain, indirectZ_ft_domain;
  sopt::OperatorFunction<T> directFFT, indirectFFT;
  const t_int imsizex_upsampled = std::floor(imsizex * oversample_ratio_image_domain);
  const t_int imsizey_upsampled = std::floor(imsizey * oversample_ratio_image_domain);
  const t_int ftsizeu_cropped = std::floor(imsizex * oversample_ratio);
  const t_int ftsizev_cropped = std::floor(imsizey * oversample_ratio);
  const t_int ftsizeu =
      std::floor(imsizex * oversample_ratio_image_domain * oversample_ratio_image_domain);
  const t_int ftsizev =
      std::floor(imsizey * oversample_ratio_image_domain * oversample_ratio_image_domain);
  PURIFY_LOW_LOG("Building Measurement Operator with Sampling on the Sphere");
  PURIFY_LOW_LOG(
      "Constructing Zero Padding "
      "and Correction Operator: "
      "ZDB");
  PURIFY_MEDIUM_LOG("Image size (width, height): {} x {}", imsizex, imsizey);
  PURIFY_MEDIUM_LOG("Oversampling Factor of FT grid: {}", oversample_ratio);
  PURIFY_MEDIUM_LOG("Oversampling Factor of Image grid: {}", oversample_ratio_image_domain);
  // Need to check relation between cell size and upsampling ratio... it will not work for large
  // fields of view! Needs to be done directly to L and M
  const Image<t_complex> S_l = purify::details::init_correction2d(
      oversample_ratio, imsizey_upsampled, imsizex_upsampled,
      [oversample_ratio_image_domain, &ftkernelu](t_real x) {
        return ftkernelu(x / oversample_ratio_image_domain);
      },
      [oversample_ratio_image_domain, &ftkernelv](t_real x) {
        return ftkernelv(x / oversample_ratio_image_domain);
      },
      w_mean, cellx / oversample_ratio_image_domain, celly / oversample_ratio_image_domain);

  std::tie(directZ_image_domain, indirectZ_image_domain) =
      purify::operators::init_zero_padding_2d<T>(S_l, oversample_ratio);

  const Image<t_complex> S_u = purify::details::init_correction2d(
      oversample_ratio_image_domain, ftsizev_cropped, ftsizeu_cropped,
      [oversample_ratio, &ftkernell](t_real x) { return ftkernell(x / oversample_ratio); },
      [oversample_ratio, &ftkernelm](t_real x) { return ftkernelm(x / oversample_ratio); }, 0., 0.,
      0.);

  std::tie(directZ_ft_domain, indirectZ_ft_domain) =
      spherical_resample::init_FT_zero_padding_2d<T>(S_u, oversample_ratio_image_domain);

  PURIFY_LOW_LOG("Constructing FFT operator: F");
  switch (ft_plan) {
  case operators::fftw_plan::measure:
    PURIFY_MEDIUM_LOG("Measuring Plans...");
    break;
  case operators::fftw_plan::estimate:
    PURIFY_MEDIUM_LOG("Estimating Plans...");
    break;
  }
  std::tie(directFFT, indirectFFT) = purify::operators::init_FFT_2d<T>(
      imsizey_upsampled, imsizex_upsampled, oversample_ratio, ft_plan);

  auto direct = sopt::chained_operators<T>(directZ_ft_domain, directFFT, directZ_image_domain);
  auto indirect =
      sopt::chained_operators<T>(indirectZ_image_domain, indirectFFT, indirectZ_ft_domain);
  return std::make_tuple(direct, indirect);
}

}  // namespace spherical_resample
}  // namespace purify
#endif
