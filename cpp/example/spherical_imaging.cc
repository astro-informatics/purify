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
  ARGS_MACRO(uvw_stacking, 8, 0., bool)
#undef ARGS_MACRO

  const t_int num_theta = 1024;
  const t_int num_phi = 512;
  const t_int number_of_samples = num_theta * num_phi;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_int Jw = 100;
  const t_real oversample_ratio_image_domain = 2;
  const t_real oversample_ratio = 2;
  const kernels::kernel kernel = kernels::kernel::kb;
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;

  const t_int num_vis = 1;

  const Vector<t_real> u = Vector<t_real>::Constant(num_vis, u_val);
  const Vector<t_real> v = Vector<t_real>::Constant(num_vis, v_val);
  const Vector<t_real> w = Vector<t_real>::Constant(num_vis, w_val);
  const Vector<t_complex> weights = Vector<t_complex>::Constant(num_vis, 1.);

  PURIFY_LOW_LOG("Resolution : {} x {} (deg x deg)", 360. / num_theta, 180. / num_phi);

  const auto phi = [num_phi](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };
  //
  t_real const offset_dec = constant::pi * 0. / 180.;
  t_real const offset_ra = constant::pi * 0. / 180.;
  const auto measure_op =
      spherical_resample::base_plane_degrid_wproj_operator<Vector<t_complex>,
                                                           std::function<t_real(t_int)>>(
          number_of_samples, theta_0 + offset_ra, phi_0 + offset_dec, theta, phi,
          spherical_resample::calculate_rotated_l(u, v, w, 0., -offset_dec, -offset_ra),
          spherical_resample::calculate_rotated_m(u, v, w, 0., -offset_dec, -offset_ra),
          spherical_resample::calculate_rotated_n(u, v, w, 0., -offset_dec, -offset_ra), weights,
          oversample_ratio, oversample_ratio_image_domain, kernel, Ju, Jw, Jl, Jm, ft_plan,
          uvw_stacking, L, 1e-8, 1e-8);

  sopt::LinearTransform<Vector<t_complex>> const m_op = sopt::LinearTransform<Vector<t_complex>>(
      std::get<0>(measure_op), {0, 1, number_of_samples}, std::get<1>(measure_op), {0, 1, num_vis});
  const Vector<t_complex> output = m_op.adjoint() * (Vector<t_complex>::Ones(1));

  Vector<t_real> l = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> m = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> n = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> th = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> ph = Vector<t_real>::Zero(number_of_samples);
  Vector<t_complex> fourier_mode = Vector<t_complex>::Zero(number_of_samples);
  for (t_int index = 0; index < number_of_samples; index++) {
    l(index) = spherical_resample::calculate_l(theta(index), phi(index), theta_0, phi_0, 0.);
    m(index) = spherical_resample::calculate_m(theta(index), phi(index), theta_0, phi_0, 0.);
    n(index) = spherical_resample::calculate_n(theta(index), phi(index), theta_0, phi_0, 0.);
    th(index) = theta(index);
    ph(index) = phi(index);
  }
  const Vector<t_real> mask = spherical_resample::generate_mask(l, m, n, L, M);
  for (t_int index = 0; index < number_of_samples; index++) {
    fourier_mode(index) =
        //       (mask(index) > 0)
        // ?
        std::conj(
            std::exp(-2 * constant::pi * t_complex(0., 1.) *
                     (l(index) * u(0) + m(index) * v(0) +
                      (std::sqrt(1 - l(index) * l(index) - m(index) * m(index)) - 1) * w(0))));
    //         : 0.;
  }

  pfitsio::write2d(Image<t_real>::Map(l.data(), num_theta, num_phi), "l_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(m.data(), num_theta, num_phi), "m_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(n.data(), num_theta, num_phi), "n_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(mask.data(), num_theta, num_phi), "mask_coordinates.fits");

  const Vector<t_real> diff = 2 * (fourier_mode - output).array().abs() /
                              (fourier_mode.array().abs() + output.array().abs());
  pfitsio::write2d(Image<t_real>::Map(diff.data(), num_theta, num_phi), "fourier_mode_diff.fits");

  pfitsio::write2d(Image<t_complex>::Map(output.data(), num_theta, num_phi).real(),
                   "fourier_mode_real_calculated.fits");
  pfitsio::write2d(Image<t_complex>::Map(output.data(), num_theta, num_phi).imag(),
                   "fourier_mode_imag_calculated.fits");
  pfitsio::write2d(Image<t_complex>::Map(fourier_mode.data(), num_theta, num_phi).real(),
                   "fourier_mode_real_expected.fits");
  pfitsio::write2d(Image<t_complex>::Map(fourier_mode.data(), num_theta, num_phi).imag(),
                   "fourier_mode_imag_expected.fits");
}
