#include "purify/config.h"
#include "purify/types.h"
#include "purify/cimg.h"
#include "purify/directories.h"
#include "purify/pfitsio.h"
#include "purify/spherical_resample.h"
using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
#define ARGS_MACRO(NAME, ARGN, VALUE, TYPE)                                                  \
  auto const NAME = static_cast<TYPE>(                                                       \
      (nargs > ARGN) ? std::stod(static_cast<std::string>(args[ARGN])) / 180. * constant::pi \
                     : VALUE);

  ARGS_MACRO(theta_0, 1, constant::pi, t_real)
  ARGS_MACRO(phi_0, 2, constant::pi / 2., t_real)
#undef ARGS_MACRO
  const t_int width = 1024;
  const t_int height = width / 2;
  const t_real dtheta = 2 * constant::pi / width;
  const t_real dphi = constant::pi / height;
  Image<t_real> l = Image<t_real>::Zero(height, width);
  Image<t_real> m = Image<t_real>::Zero(height, width);
  Image<t_real> n = Image<t_real>::Zero(height, width);
  for (t_int i = 0; i < width; i++) {
    for (t_int j = 0; j < height; j++) {
      l(j, i) = spherical_resample::calculate_l(theta_0, phi_0, dtheta * i, dphi * j);
      m(j, i) = spherical_resample::calculate_m(theta_0, phi_0, dtheta * i, dphi * j);
      n(j, i) = spherical_resample::calculate_n(theta_0, phi_0, dtheta * i, dphi * j);
    }
  }
  pfitsio::write2d(l.transpose(), "l_coordinates.fits");
  pfitsio::write2d(m.transpose(), "m_coordinates.fits");
  pfitsio::write2d(n.transpose(), "n_coordinates.fits");
}
