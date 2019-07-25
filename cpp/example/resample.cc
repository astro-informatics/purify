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
#undef ARGS_MACRO

  std::string const fitsfile = image_filename("M31.fits");
  Image<t_complex> const M31 = pfitsio::read2d(fitsfile);
  Vector<t_complex> const image = Vector<t_complex>::Map(M31.data(), M31.size());
  const t_int num_theta = 1024;
  const t_int num_phi = 512;
  const t_int number_of_samples = num_theta * num_phi;
  const t_int imsizey = M31.rows();
  const t_int imsizex = M31.cols();
  const t_real dl = L / imsizex;
  const t_real dm = M / imsizey;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_real oversample_ratio_image_domain = 1;

  const auto kernelstuff =
      purify::create_kernels(kernels::kernel_from_string.at("kb"), Jl, Jm, imsizey, imsizex, 1);
  const std::function<t_real(t_real)>& kernell = std::get<0>(kernelstuff);
  const std::function<t_real(t_real)>& kernelm = std::get<1>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernell = std::get<2>(kernelstuff);
  const std::function<t_real(t_real)>& ftkernelm = std::get<3>(kernelstuff);

  const auto phi = [num_phi](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };
  const auto resample_operator =
      spherical_resample::init_mask_and_resample_operator_2d<Vector<t_complex>,
                                                             std::function<t_real(t_int)>>(
          number_of_samples, theta_0, phi_0, theta, phi, imsizey, imsizex,
          oversample_ratio_image_domain, dl, dm, kernell, kernelm, Jl, Jm);
  sopt::LinearTransform<Vector<t_complex>> const resampler =
      sopt::LinearTransform<Vector<t_complex>>(
          std::get<0>(resample_operator), {0, 1, number_of_samples}, std::get<1>(resample_operator),
          {0, 1, imsizex * imsizey});
  const Vector<t_complex> output = resampler.adjoint() * image;

  Vector<t_real> l = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> m = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> n = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> th = Vector<t_real>::Zero(number_of_samples);
  Vector<t_real> ph = Vector<t_real>::Zero(number_of_samples);
  for (t_int index = 0; index < number_of_samples; index++) {
    l(index) = spherical_resample::calculate_l(theta(index), phi(index), 0., phi_0, theta_0);
    m(index) = spherical_resample::calculate_m(theta(index), phi(index), 0., phi_0, theta_0);
    n(index) = spherical_resample::calculate_n(theta(index), phi(index), 0., phi_0, theta_0);
    th(index) = theta(index);
    ph(index) = phi(index);
  }
  Vector<t_real> mask = spherical_resample::generate_mask(l, m, n, imsizex, imsizey, dl, dm);

  pfitsio::write2d(Image<t_complex>::Map(output.data(), num_theta, num_phi).real(),
                   "image_on_sphere.fits");
  pfitsio::write2d(Image<t_real>::Map(l.data(), num_theta, num_phi), "l_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(m.data(), num_theta, num_phi), "m_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(n.data(), num_theta, num_phi), "n_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(th.data(), num_theta, num_phi), "theta_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(ph.data(), num_theta, num_phi), "phi_coordinates.fits");
  pfitsio::write2d(Image<t_real>::Map(mask.data(), num_theta, num_phi), "mask_coordinates.fits");
}
