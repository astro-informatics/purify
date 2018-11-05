#include "wkernel_integration.h"
#include <boost/math/special_functions/bessel.hpp>

namespace purify {

namespace projection_kernels {
t_complex phase(const t_real u, const t_real v, const t_real l, const t_real m) {
  return std::exp(t_complex(0., -2 * constant::pi * (u * l + v * m)));
}
t_complex chirp(const t_real w, const t_real l, const t_real m) {
  return std::exp(t_complex(0., -2 * constant::pi * w * (std::sqrt(1 - l * l - m * m) - 1)));
}
t_complex exact_w_projection_integration_1d(const t_real u, const t_real v, const t_real w,
                                            const t_real du, const t_real oversample_ratio,
                                            const std::function<t_complex(t_real)> &ftkerneluv,
                                            const t_uint &max_evaluations,
                                            const t_real &absolute_error,
                                            const t_real &relative_error,
                                            const integration::method method, t_uint &evaluations) {
  if (oversample_ratio != 2)
    throw std::runtime_error("Oversample ratio != 2 not tested for wproj.");
  evaluations = 0;
  const auto func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    assert(std::abs(x(0)) <= 1);
    return ftkerneluv(x(0)) * chirp(w, x(0) / du, 0) *
           boost::math::cyl_bessel_j(0, 2 * constant::pi * x(0) * std::sqrt(u * u + v * v)) * x(0);
    // Hankel transform
  };
  const Vector<t_real> xmin = Vector<t_real>::Zero(1);
  const Vector<t_real> xmax = Vector<t_real>::Constant(1, std::max(oversample_ratio / 2., du));
  return 2. * constant::pi *
         integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method) /
         std::pow(xmax(0), 2);
}

t_complex exact_w_projection_integration(const t_real u, const t_real v, const t_real w,
                                         const t_real du, const t_real dv,
                                         const t_real oversample_ratio,
                                         const std::function<t_complex(t_real)> &ftkernelu,
                                         const std::function<t_complex(t_real)> &ftkernelv,
                                         const t_uint &max_evaluations,
                                         const t_real &absolute_error, const t_real &relative_error,
                                         const integration::method method, t_uint &evaluations) {
  if (oversample_ratio != 2)
    throw std::runtime_error("Oversample ratio != 2 not tested for wproj.");
  auto const func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    return ftkernelu(x(0)) * ftkernelv(x(1)) * chirp(w, 1. / du * x(0), 1. / dv * x(1)) *
           phase(u, v, x(0), x(1)) *
           // checking bounds don't go past the horizon...
           (((std::pow(x(0) / du, 2) + std::pow(x(1) / dv, 2)) < 1) ? 1. : 0.);
  };
  Vector<t_real> xmax = Vector<t_real>::Constant(2, oversample_ratio / 2.);
  const Vector<t_real> xmin = -xmax;
  return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method) /
         std::max(oversample_ratio, oversample_ratio / du) /
         std::max(oversample_ratio, oversample_ratio / dv);
}
}  // namespace projection_kernels
}  // namespace purify
