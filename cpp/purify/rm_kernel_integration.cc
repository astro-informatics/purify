#include "rm_kernel_integration.h"
#include <boost/math/special_functions/sinc.hpp>

namespace purify {

namespace projection_kernels {
t_complex fourier_rm_kernel(const t_real x, const t_real dlambda2, const t_real u,
                            const t_real du) {
  return std::exp(t_complex(0., -2 * constant::pi * u * x)) *
         ((dlambda2 > 0) ? boost::math::sinc_pi(dlambda2 * x * constant::pi / du) : 1.);
}

t_complex exact_rm_projection_integration(const t_real u, const t_real dlambda2, const t_real du,
                                          const t_real oversample_ratio,
                                          const std::function<t_complex(t_real)> &ftkernelu,
                                          const t_uint max_evaluations,
                                          const t_real absolute_error,
                                          const t_real relative_error,
                                          const integration::method method, t_uint &evaluations) {
  auto const func = [&](const Vector<t_real> &x) -> t_complex {
    evaluations++;
    return ftkernelu(x(0)) * fourier_rm_kernel(x(0), dlambda2, u, du);
  };
  Vector<t_real> xmax = Vector<t_real>::Zero(1);
  xmax(0) = oversample_ratio / 2.;
  const Vector<t_real> xmin = -xmax;
  return integration::integrate(xmin, xmax, func, integration::norm_type::paired, absolute_error,
                                relative_error, max_evaluations, method) /
         (xmax(0) - xmin(0));
}
}  // namespace projection_kernels
}  // namespace purify
