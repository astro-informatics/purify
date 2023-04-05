#include "purify/integration.h"

namespace purify {
namespace integration {

t_real integrate(const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                 const std::function<t_real(Vector<t_real>)> &func, const norm_type norm,
                 const t_real required_abs_error, const t_real required_rel_error,
                 const t_uint max_evaluations, const method methodtype) {
  assert(xmin.size() == xmax.size());
  t_real val = 0;
  t_real err = 0;
  auto wrap_integrand = [](unsigned ndim, const t_real *x, void *fdata, unsigned fdim,
                           double *fval) -> int {
    fval[0] = (*(static_cast<std::function<t_real(Vector<t_real>)> *>(fdata)))(
        Vector<t_real>::Map(x, ndim));
    return 0;
  };
  switch (methodtype) {
  case method::p:
    if (pcubature(1, wrap_integrand, const_cast<std::function<t_real(Vector<t_real>)> *>(&func),
                  xmin.size(), xmin.data(), xmax.data(), max_evaluations, required_abs_error,
                  required_rel_error, norm_error(norm), &val, &err) != 0)
      throw std::runtime_error("Error in calculating p adaptive integral with cubature.");
    break;
  case method::h:
    if (hcubature(1, wrap_integrand, const_cast<std::function<t_real(Vector<t_real>)> *>(&func),
                  xmin.size(), xmin.data(), xmax.data(), max_evaluations, required_abs_error,
                  required_rel_error, norm_error(norm), &val, &err) != 0)
      throw std::runtime_error("Error in calculating h adaptive integral with cubature.");
    break;
  default:
    throw std::runtime_error("Method not possibe with cubature.");
    break;
  }
  return val;
}

t_complex integrate(const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                    const std::function<t_complex(Vector<t_real>)> &func, const norm_type norm,
                    const t_real required_abs_error, const t_real required_rel_error,
                    const t_uint max_evaluations, const method methodtype) {
  assert(xmin.size() == xmax.size());
  std::array<t_real, 2> val{{0., 0.}};
  std::array<t_real, 2> err{{0., 0.}};
  auto wrap_integrand = [](unsigned ndim, const t_real *x, void *fdata, unsigned fdim,
                           double *fval) -> int {
    assert(fdim == 2);
    const t_complex output = (*(static_cast<std::function<t_complex(Vector<t_real>)> *>(fdata)))(
        Vector<t_real>::Map(x, ndim));
    fval[0] = output.real();
    fval[1] = output.imag();
    return 0;
  };
  switch (methodtype) {
  case method::p:
    if (pcubature(val.size(), wrap_integrand,
                  const_cast<std::function<t_complex(Vector<t_real>)> *>(&func), xmin.size(),
                  xmin.data(), xmax.data(), max_evaluations, required_abs_error, required_rel_error,
                  norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating p adaptive integral with cubature.");
    break;
  case method::h:
    if (hcubature(val.size(), wrap_integrand,
                  const_cast<std::function<t_complex(Vector<t_real>)> *>(&func), xmin.size(),
                  xmin.data(), xmax.data(), max_evaluations, required_abs_error, required_rel_error,
                  norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating h adaptive integral with cubature.");
    break;
  default:
    throw std::runtime_error("Integration method not known with cubature.");
    break;
  }

  return t_complex(val.at(0), val.at(1));
}
Vector<t_real> integrate(const t_uint fdim, const Vector<t_real> &xmin, const Vector<t_real> &xmax,
                         const std::function<Vector<t_real>(Vector<t_real>)> &func,
                         const norm_type norm, const t_real required_abs_error,
                         const t_real required_rel_error, const t_uint max_evaluations,
                         const method methodtype) {
  assert(xmin.size() == xmax.size());
  Vector<t_real> val = Vector<t_real>::Zero(fdim);
  Vector<t_real> err = Vector<t_real>::Zero(fdim);
  auto wrap_integrand = [](unsigned ndim, const t_real *x, void *fdata, unsigned fdim,
                           double *fval) -> int {
    Vector<t_real>::Map(fval, fdim) =
        (*(static_cast<std::function<Vector<t_real>(Vector<t_real>)> *>(fdata)))(
            Vector<t_real>::Map(x, ndim));
    return 0;
  };
  switch (methodtype) {
  case method::p:
    if (pcubature(val.size(), wrap_integrand,
                  const_cast<std::function<Vector<t_real>(Vector<t_real>)> *>(&func), xmin.size(),
                  xmin.data(), xmax.data(), max_evaluations, required_abs_error, required_rel_error,
                  norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating p adaptive integral with cubature.");
    break;
  case method::h:
    if (hcubature(val.size(), wrap_integrand,
                  const_cast<std::function<Vector<t_real>(Vector<t_real>)> *>(&func), xmin.size(),
                  xmin.data(), xmax.data(), max_evaluations, required_abs_error, required_rel_error,
                  norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating h adaptive integral with cubature.");
    break;
  default:
    throw std::runtime_error("Method not possible with cubature.");
    break;
  }

  return val;
}
Vector<t_complex> integrate_v(const t_uint fdim, const t_uint npts, const Vector<t_real> &xmin,
                              const Vector<t_real> &xmax,
                              const std::vector<std::function<t_complex(Vector<t_real>)>> &func,
                              const norm_type norm, const t_real required_abs_error,
                              const t_real required_rel_error, const t_uint max_evaluations,
                              const method methodtype) {
  assert(xmin.size() == xmax.size());
  Vector<t_real> val = Vector<t_real>::Zero(2 * fdim);
  Vector<t_real> err = Vector<t_real>::Zero(2 * fdim);
  auto wrap_integrand = [](unsigned ndim, unsigned long npts, const t_real *x, void *fdata,
                           unsigned fdim, double *fval) -> int {
    for (int i = 0; i < npts; i = i + 2) {
      const t_complex val =
          (*(static_cast<std::vector<std::function<t_complex(Vector<t_real>)>> *>(fdata)))
              .at(i)(Vector<t_real>::Map(x, ndim));
      fval[i] = val.real();
      fval[i + 1] = val.imag();
    }
    return 0;
  };
  switch (methodtype) {
  case method::p:
    if (pcubature_v(val.size(), wrap_integrand,
                    const_cast<std::vector<std::function<t_complex(Vector<t_real>)>> *>(&func),
                    xmin.size(), xmin.data(), xmax.data(), max_evaluations, required_abs_error,
                    required_rel_error, norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating p adaptive integral with cubature.");
    break;
  case method::h:
    if (hcubature_v(val.size(), wrap_integrand,
                    const_cast<std::vector<std::function<t_complex(Vector<t_real>)>> *>(&func),
                    xmin.size(), xmin.data(), xmax.data(), max_evaluations, required_abs_error,
                    required_rel_error, norm_error(norm), val.data(), err.data()) != 0)
      throw std::runtime_error("Error in calculating h adaptive integral with cubature.");
    break;
  default:
    throw std::runtime_error("Method not possible with cubature.");
    break;
  }
  Vector<t_complex> output = Vector<t_complex>::Zero(fdim);
  output.real() = Vector<t_real>::Map(val.data(), fdim, 1, Eigen::Stride<Eigen::Dynamic, 1>(2, 0));
  output.imag() =
      Vector<t_real>::Map(val.data() + 1, fdim, 1, Eigen::Stride<Eigen::Dynamic, 1>(2, 0));
  return output;
}

error_norm norm_error(norm_type norm) {
  switch (norm) {
  case norm_type::l1:
    return ERROR_L1;
    break;
  case norm_type::l2:
    return ERROR_L2;
    break;
  case norm_type::linf:
    return ERROR_LINF;
    break;
  case norm_type::individual:
    return ERROR_INDIVIDUAL;
    break;
  case norm_type::paired:
    return ERROR_PAIRED;
    break;
  default:
    return ERROR_L2;
    break;
  }
}
t_complex convolution(const Vector<t_real> &x0, const Vector<t_real> &xmin,
                      const Vector<t_real> &xmax,
                      const std::function<t_complex(Vector<t_real>)> &func1,
                      const std::function<t_complex(Vector<t_real>)> &func2, const norm_type norm,
                      const t_real required_abs_error, const t_real required_rel_error,
                      const t_uint max_evaluations, const method methodtype) {
  return integrate(
      xmin, xmax, [=](const Vector<t_real> &x) -> t_complex { return func1(x0 - x) * func2(x); },
      norm, required_abs_error, required_rel_error, max_evaluations, methodtype);
}
}  // namespace integration
}  // namespace purify
