#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/utilities.h"

#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/pfitsio.h"
#include "purify/spherical_resample.h"
using namespace purify;
using namespace purify::notinstalled;

using namespace purify;

int main(int nargs, char const** args) {
  purify::logging::initialize();
  purify::logging::set_level("debug");
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE)                                                  \
  auto const NAME = static_cast<TYPE>(                                                       \
      (nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) / 180. * constant::pi \
                     : VALUE);

  ARGS_MACRO(theta_0, 1, 0., t_real)
  ARGS_MACRO(phi_0, 2, constant::pi / 2., t_real)
#undef ARGS_MACRO
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE) \
  auto const NAME =                         \
      static_cast<TYPE>((nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) : VALUE);

  ARGS_MACRO(L, 3, 1., t_real)
  ARGS_MACRO(M, 4, 1., t_real)
  ARGS_MACRO(u_val, 5, 0., t_real)
  ARGS_MACRO(v_val, 6, 0., t_real)
  ARGS_MACRO(w_val, 7, 0., t_real)
#undef ARGS_MACRO

  const t_int num_theta = 1024;
  const t_int num_phi = 512;
  const t_int number_of_samples = num_theta * num_phi;
  const t_int imsizey = 256;
  const t_int imsizex = 256;
  const t_real dl = L / imsizex;
  const t_real dm = M / imsizey;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_real oversample_ratio_image_domain = 2;
  const t_real oversample_ratio = 2;
  const kernels::kernel kernel = kernels::kernel::kb;
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  const bool w_stacking = true;

  const t_int num_vis = 1;

  const Vector<t_real> u = Vector<t_real>::Constant(num_vis, u_val);
  const Vector<t_real> v = Vector<t_real>::Constant(num_vis, v_val);
  const Vector<t_real> w = Vector<t_real>::Constant(num_vis, w_val);
  const Vector<t_complex> weights = Vector<t_complex>::Constant(num_vis, 1);

  const auto phi = [num_phi](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };
  const auto measure_op =
      spherical_resample::base_plane_degrid_operator<Vector<t_complex>,
                                                     std::function<t_real(t_int)>>(
          number_of_samples, theta_0, phi_0, theta, phi, u, v, w, weights, imsizey, imsizex,
          oversample_ratio, oversample_ratio_image_domain, kernel, Ju, Jv, Jl, Jm, ft_plan,
          w_stacking, dl, dm);

  sopt::LinearTransform<Vector<t_complex>> const m_op = sopt::LinearTransform<Vector<t_complex>>(
      std::get<0>(measure_op), {0, 1, number_of_samples}, std::get<1>(measure_op), {0, 1, num_vis});
  const Vector<t_complex> output = m_op.adjoint() * Vector<t_complex>::Ones(1);

  pfitsio::write2d(Image<t_complex>::Map(output.data(), num_theta, num_phi).real(),
                   "image_on_sphere.fits");
}
