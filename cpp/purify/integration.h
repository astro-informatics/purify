#ifndef PURIFY_INTEGRATION_H
#define PURIFY_INTEGRATION_H

#include "purify/config.h"
#include "purify/types.h"
#include <cubature.h>
#include "purify/logging.h"

namespace purify {
namespace integration {
enum class norm_type { individual, paired, l1, l2, linf };
enum class method { h, p };
//! return norm used for error
error_norm norm_error(norm_type norm);
//!  adaptive integration with cubature for real scalar to vector
t_real integrate(const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                 const std::function<t_real(Vector<t_real>)> &func, const norm_type norm,
                 const t_real required_abs_error, const t_real required_rel_error,
                 const t_uint max_evaluations, const method methodtype);
//!  adaptive integration with cubature for complex scalar to vector
t_complex integrate(const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                    const std::function<t_complex(Vector<t_real>)> &func, const norm_type norm,
                    const t_real required_abs_error, const t_real required_rel_error,
                    const t_uint max_evaluations, const method methodtype);
//! adaptive integration with cubature for vector to vector
Vector<t_real> integrate(const t_uint fdim, const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                         const std::function<Vector<t_real>(Vector<t_real>)> &func,
                         const norm_type norm, const t_real required_abs_error,
                         const t_real required_rel_error, const t_uint max_evaluations,
                         const method methodtype);
//! adaptive integration with cubature for vector to vector using SIMD
Vector<t_complex> integrate_v(const t_uint fdim, const Vector<t_real> &xmin,
                              const Vector<t_real> &xmax,
                              const std::vector<std::function<t_complex(Vector<t_real>)>> &func,
                              const norm_type norm, const t_real required_abs_error,
                              const t_real required_rel_error, const t_uint max_evaluations,
                              const method methodtype);
//! use adaptive integration to calculate convolution at x0 of func1(x0 - x) * func2(x)
t_complex convolution(const Vector<t_real> &x0, const Vector<t_real> &xmin,
                      const Vector<t_real> &xmax,
                      const std::function<t_complex(Vector<t_real>)> &func1,
                      const std::function<t_complex(Vector<t_real>)> &func2, const norm_type norm,
                      const t_real required_abs_error, const t_real required_rel_error,
                      const t_uint max_evaluations, const method methodtype);
//! use adaptive integration to calculate convolution at x0 of func1(x0 - x) * func2(x) from -∞
//! to +∞
t_complex convolution(const Vector<t_real> &x0,
                      const std::function<t_complex(Vector<t_real>)> &func1,
                      const std::function<t_complex(Vector<t_real>)> &func2, const norm_type norm,
                      const t_real required_abs_error, const t_real required_rel_error,
                      const t_uint max_evaluations, const method methodtype);

}  // namespace integration
}  // namespace purify

#endif
