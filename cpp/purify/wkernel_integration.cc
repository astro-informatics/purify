#include "wkernel_integration.h"
#include <boost/math/special_functions/bessel.hpp>

namespace purify {

namespace projection_kernels {
t_complex phase(const t_real u, const t_real v, const t_real l, const t_real m) {
  return std::exp(t_complex(0., -2 * constant::pi * (u * l + v * m))) / (2 * constant::pi);
}
t_complex chirp(const t_real w, const t_real l, const t_real m) {
  return (l * l + m * m <= 1)
             ? std::exp(t_complex(
                   0., -2 * constant::pi * w *
                           (std::sqrt(1 - l * l - m * m) - 1))) /* / std::sqrt(1 - l * l - m * m) */
             : 0.;
}
t_complex exact_w_projection_integration_1d(const t_real &u, const t_real &v, const t_real &w,
                                            const t_real du,
                                            const std::function<t_complex(t_real)> &ftkerneluv,
                                            const t_uint &max_evaluations,
                                            const t_real &absolute_error,
                                            const t_real &relative_error,
                                            const integration::method method, t_uint &evaluations) {
  evaluations = 0;
  const auto func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    assert(std::abs(x(0)) <= 1);
    return 2 * constant::pi * ftkerneluv(x(0)) * chirp(w, 1. / du * x(0), 0) *
           boost::math::cyl_bessel_j(0, 2 * constant::pi * x(0) * std::sqrt(u * u + v * v)) * 0.25 *
           x(0);
    // Hankel transform
  };
  const Vector<t_real> xmin = Vector<t_real>::Zero(1);
  Vector<t_real> xmax = Vector<t_real>::Zero(1);
  xmax(0) = std::max(1., du);
  return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method);
}

t_complex exact_w_projection_integration(const t_real &u, const t_real &v, const t_real &w,
                                         const t_real du, const t_real dv,
                                         const std::function<t_complex(t_real)> &ftkernelu,
                                         const std::function<t_complex(t_real)> &ftkernelv,
                                         const t_uint &max_evaluations,
                                         const t_real &absolute_error, const t_real &relative_error,
                                         const integration::method method, const bool radial,
                                         t_uint &evaluations) {
  if (radial) {
    evaluations = 0;
    const auto func = [&](const Vector<t_real> &x) -> t_complex {
      evaluations++;
      assert(std::abs(x(0)) <= 1);
      return ftkernelu(x(0) * std::cos(x(1))) * ftkernelv(x(0) * std::sin(x(1))) *
             chirp(w, 1. / du * x(0) * std::cos(x(1)), 1. / dv * x(0) * std::sin(x(1))) *
             phase(u, v, x(0) * std::cos(x(1)), x(0) * std::sin(x(1))) * 0.25 * x(0);
      // the radius at the end is the element of area in polar coordinates
    };
    const Vector<t_real> xmin = Vector<t_real>::Zero(2);
    Vector<t_real> xmax = Vector<t_real>::Zero(2);
    xmax(0) = 1;
    xmax(1) = 2 * constant::pi;
    return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                  relative_error, max_evaluations, method);
  } else {
    auto const func = [&](const Vector<t_real> &x) -> t_complex {
      evaluations++;
      return ftkernelu(x(0)) * ftkernelv(x(1)) * chirp(w, 1. / du * x(0), 1. / dv * x(1)) *
             phase(u, v, x(0), x(1)) * 0.25;
    };
    Vector<t_real> xmax = Vector<t_real>::Ones(2);
    const Vector<t_real> xmin = -xmax;
    // checking bounds don't go past the horizon...
    return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                  relative_error, max_evaluations, method);
  }
}
}  // namespace projection_kernels
}  // namespace purify
