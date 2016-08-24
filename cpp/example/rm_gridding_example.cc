
#include "purify/RMOperator.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"
using namespace purify;

int main(int nargs, char const **args) {

  utilities::rm_params rm_vis;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;

  // Gridding example
  cellsize = 0.1; // in rad/m^2
  over_sample = 2;
  t_int J = 4;
  t_int width = 2048;
  kernel = "kb";
  t_int number_of_samples = 100;
  t_real RM1 = -70;
  t_real RM2 = 50;

  rm_vis.frequency.setLinSpaced(number_of_samples, 100, 110);
  Array<t_real> wavelength_squared
      = constant::c * constant::c / (rm_vis.frequency.array() * rm_vis.frequency.array()) * 1e-12;
  t_complex I(0, 1);
  rm_vis.linear_polarisation
      = exp(-2. * I * RM1 * wavelength_squared)
        + exp(-2. * I * RM2 * wavelength_squared); // rotation measure component
  rm_vis.weights = rm_vis.linear_polarisation.array() * 0. + 1.;
  RMOperator op(rm_vis, J, kernel, width, over_sample, cellsize, "none",
                0); // Generating gridding matrix

  auto faraday_dispersion = op.grid(rm_vis.linear_polarisation);

  Image<t_real> output = faraday_dispersion.array().abs();
  pfitsio::write2d(output, "rm_kb_4.fits");
}
