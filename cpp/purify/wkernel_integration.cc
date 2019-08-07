#include "wkernel_integration.h"
#include <boost/math/special_functions/bessel.hpp>

namespace purify {

namespace projection_kernels {
t_complex fourier_wproj_kernel(const t_real x, const t_real y, const t_real w, const t_real u,
                               const t_real v, const t_real du, const t_real dv) {
  // checking bounds don't go past the horizon...
  if ((std::pow(x / du, 2) + std::pow(y / dv, 2)) < 1)
    return std::exp(t_complex(
        0.,
        -2 * constant::pi *
            (u * x + v * y + w * (std::sqrt(1 - (x * x) / (du * du) - (y * y) / (dv * dv)) - 1))));
  else
    return 0;
}
t_complex hankel_wproj_kernel(const t_real r, const t_real w, const t_real u, const t_real v,
                              const t_real du) {
  return ((r / du < 1. and r < 0.5)
              ? std::exp(
                    t_complex(0., -2 * constant::pi * w * (std::sqrt(1 - (r * r) / (du * du)) - 1)))
              : 1.) *
         boost::math::cyl_bessel_j(0, 2 * constant::pi * r * std::sqrt(u * u + v * v)) * r;
}
t_complex exact_w_projection_integration_1d(const t_real u, const t_real v, const t_real w,
                                            const t_real du, const t_real oversample_ratio,
                                            const std::function<t_complex(t_real)> &ftkerneluv,
                                            const t_uint &max_evaluations,
                                            const t_real &absolute_error,
                                            const t_real &relative_error,
                                            const integration::method method, t_uint &evaluations) {
  evaluations = 0;
  const auto func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    assert(std::abs(x(0)) <= 1);
    return ftkerneluv(x(0)) * hankel_wproj_kernel(x(0), w, u, v, du);
  };
  const Vector<t_real> xmin = Vector<t_real>::Zero(1);
  const Vector<t_real> xmax = Vector<t_real>::Constant(1, oversample_ratio * 0.5);
  return 2. * constant::pi *
         integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method);
}

t_complex exact_w_projection_integration(const t_real u, const t_real v, const t_real w,
                                         const t_real du, const t_real dv,
                                         const t_real oversample_ratio,
                                         const std::function<t_complex(t_real)> &ftkernelu,
                                         const std::function<t_complex(t_real)> &ftkernelv,
                                         const t_uint &max_evaluations,
                                         const t_real &absolute_error, const t_real &relative_error,
                                         const integration::method method, t_uint &evaluations) {
  auto const func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    return ftkernelu(x(0)) * ftkernelv(x(1)) * fourier_wproj_kernel(x(0), x(1), w, u, v, du, dv);
  };
  Vector<t_real> xmax = Vector<t_real>::Zero(2);
  xmax(0) = oversample_ratio / 2.;
  xmax(1) = oversample_ratio / 2.;
  const Vector<t_real> xmin = -xmax;
  return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method);
}
}  // namespace projection_kernels
}  // namespace purify
