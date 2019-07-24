#ifndef PURIFY_SPHERICAL_RESAMPLE_H
#define PURIFY_SPHERICAL_RESAMPLE_H

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
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
//! return operator that will resample between the sphere and the plane
template <class K, class T>
std::tuple<sopt::OperatorFunction<K>, sopt::OperatorFunction<K>> init_resample_operator_2d(
    const t_int number_of_samples, const t_real theta_0, const t_real phi_0, const T &theta,
    const T &phi, const t_int imsizey, const t_int imsizex, const t_real dl, const t_real dm,
    const std::function<t_real(t_real)> kernell, const std::function<t_real(t_real)> kernelm,
    const t_int Jl, const t_int Jm) {
  if ((Jl <= 0) or (Jm <= 0))
    throw std::runtime_error("Support size of resampling kernel is not positive");
  if ((imsizex <= 0) or (imsizey <= 0))
    throw std::runtime_error("Tangent image size is not greater than zero.");
  if (number_of_samples <= 0)
    throw std::runtime_error("Number of samples on the sphere is not greater than zero.");

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
  const Sparse<t_complex> interrpolation_matrix = init_resample_matrix_2d(
      l_compressed, m_compressed, imsizey, imsizex, kernell, kernelm, Jl, Jm);
  const Sparse<t_complex> interrpolation_matrix_adjoint = interrpolation_matrix.adjoint();
  return std::make_tuple(
      [=](K &output, const K &input) {
        assert(input.size() == number_of_samples);
        K buff = K::Zero(indicies.size());
        for (t_int k = 0; k < indicies.size(); k++) buff(k) = input(indicies.at(k));
        output = interrpolation_matrix_adjoint * buff;
      },
      [=](K &output, const K &input) {
        assert(input.size() == imsizex * imsizey);
        const K buff = interrpolation_matrix * input;
        output = K::Zero(number_of_samples);
        for (t_int k = 0; k < indicies.size(); k++) output(indicies.at(k)) = buff(k);
      });
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

}  // namespace spherical_resample
}  // namespace purify
#endif
