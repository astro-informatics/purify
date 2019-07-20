#include "purify/spherical_resample.h"
#include <iostream>
#include "purify/utilities.h"
namespace purify {
namespace spherical_resample {

t_real calculate_l(const t_real theta, const t_real phi) { return std::cos(theta) * std::sin(phi); }

t_real calculate_m(const t_real theta, const t_real phi) { return std::sin(theta) * std::sin(phi); }

t_real calculate_n(const t_real phi) { return std::cos(phi); }

t_real calculate_rotated_l(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma) {
  return l * (std::cos(alpha) * std::cos(beta) * std::cos(gamma) -
              std::sin(alpha) * std::sin(gamma)) +
         m * (-std::sin(alpha) * std::cos(beta) * std::cos(gamma) -
              std::cos(alpha) * std::sin(gamma)) +
         n * std::cos(alpha) * std::sin(beta);
}

t_real calculate_rotated_m(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma) {
  return l * (std::sin(alpha) * std::cos(beta) * std::cos(gamma) +
              std::cos(alpha) * std::sin(gamma)) +
         m * (-std::sin(alpha) * std::cos(beta) * std::sin(gamma) +
              std::cos(alpha) * std::cos(gamma)) +
         n * std::sin(alpha) * std::sin(beta);
}

t_real calculate_rotated_n(const t_real l, const t_real m, const t_real n, const t_real alpha,
                           const t_real beta, const t_real gamma) {
  return l * (-std::sin(beta) * std::cos(gamma)) + m * (std::sin(beta) * std::sin(gamma)) +
         n * std::cos(beta);
}

t_real calculate_l(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma) {
  return calculate_rotated_l(calculate_l(theta, phi), calculate_m(theta, phi), calculate_n(phi),
                             alpha, beta, gamma);
}

t_real calculate_m(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma) {
  return calculate_rotated_m(calculate_l(theta, phi), calculate_m(theta, phi), calculate_n(phi),
                             alpha, beta, gamma);
}

t_real calculate_n(const t_real theta, const t_real phi, const t_real alpha, const t_real beta,
                   const t_real gamma) {
  return calculate_rotated_n(calculate_l(theta, phi), calculate_m(theta, phi), calculate_n(phi),
                             alpha, beta, gamma);
}

std::vector<t_int> generate_indicies(const Vector<t_real> &l, const Vector<t_real> &m,
                                     const Vector<t_real> &n, const t_int imsizey,
                                     const t_int imsizex, const t_real dl, const t_real dm) {
  if (l.size() != m.size()) throw std::runtime_error("number of l and m samples do not match.");
  if (l.size() != n.size()) throw std::runtime_error("number of l and n samples do not match.");
  if (l.size() < 1) throw std::runtime_error("number of l is less than 1.");
  const t_real L = imsizex * dl;
  const t_real M = imsizey * dm;
  std::vector<t_int> indicies(0);
  for (t_int i = 0; i < l.size(); i++) {
    if ((std::abs(l(i)) < L * 0.5) and (std::abs(m(i)) < M * 0.5) and (n(i) > 0.)) {
      indicies.push_back(i);
    }
  }
  return indicies;
}

Sparse<t_complex> init_resample_matrix_2d(const Vector<t_real> &l, const Vector<t_real> &m,
                                          const t_int imsizey, const t_int imsizex,
                                          const std::function<t_real(t_real)> kernell,
                                          const std::function<t_real(t_real)> kernelm,
                                          const t_int Jl, const t_int Jm) {
  const t_int rows = l.size();
  const t_int cols = imsizex * imsizey;
  if (l.size() != m.size())
    throw std::runtime_error(
        "Size of l and m vectors are not the same for creating resampling matrix.");

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Jl * Jm));

  const t_int jl_max = std::min(Jl, imsizex);
  const t_int jm_max = std::min(Jm, imsizey);
  // If I collapse this for loop there is a crash when using MPI... Sparse<>::insert() doesn't work
  // right
#pragma omp parallel for
  for (t_int k = 0; k < rows; ++k) {
    for (t_int jl = 1; jl < jl_max + 1; ++jl) {
      for (t_int jm = 1; jm < jm_max + 1; ++jm) {
        const t_real k_l = std::floor(l(k) - jl_max * 0.5);
        const t_real k_m = std::floor(m(k) - jm_max * 0.5);
        const t_int q = k_l + jl + std::floor(imsizex * 0.5);
        const t_int p = k_m + jm + std::floor(imsizey * 0.5);
        const t_int index = utilities::sub2ind(p, q, imsizey, imsizex);
        assert(k >= 0);
        assert(k < rows);
        if ((cols > index) and (index >= 0))
          interpolation_matrix.insert(k, index) =
              kernell(l(k) - (k_l + jl)) * kernelm(m(k) - (k_m + jm));
      }
    }
  }
  return interpolation_matrix;
}

}  // namespace spherical_resample
}  // namespace purify
