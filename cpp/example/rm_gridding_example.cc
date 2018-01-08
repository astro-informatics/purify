
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
using namespace purify;

int main(int nargs, char const **args) {

  utilities::vis_params rm_vis;

  // Gridding example
  const t_real cellsize = 0.1; // in rad/m^2
  const t_real oversample = 2;
  const t_int J = 4;
  const t_int width = 2048;
  const std::string kernel = "kb";
  const t_int number_of_samples = 100;
  const t_real RM1 = -70;
  const t_real RM2 = 50;
  const t_real RM_centre = -100;

  rm_vis.u.setLinSpaced(number_of_samples, 100, 110);
  rm_vis.v = Vector<t_real>::Zero(rm_vis.u.size());
  rm_vis.w = rm_vis.v;
  rm_vis.units = "radians";
  const Array<t_real> wavelength_squared
      = constant::c * constant::c / (rm_vis.u.array() * rm_vis.u.array()) * 1e-12;
  const t_complex I(0, 1);
  rm_vis.vis = exp(-2. * I * RM1 * wavelength_squared)
               + exp(-2. * I * RM2 * wavelength_squared);  // rotation measure component
  rm_vis.weights = Vector<t_complex>::Ones(rm_vis.size()); // determine FoV
  auto const measurements_transform
      = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          rm_vis, width, 1, cellsize, cellsize, oversample, 100, 0.0001, kernel, J, J);

  const Vector<t_complex> faraday_dispersion = measurements_transform->adjoint() * rm_vis.vis;

  Image<t_real> output = faraday_dispersion.array().abs();
  pfitsio::write2d(output, "rm_kb_4.fits");
}
