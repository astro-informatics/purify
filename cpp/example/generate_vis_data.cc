#include "purify/config.h"
#include <array>
#include <memory>
#include <random>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

int main(int nargs, char const **args) {
  using namespace purify;
  using namespace purify::notinstalled;
  purify::logging::initialize();
  purify::logging::set_level(purify::default_logging_level());

  std::string const fitsfile = image_filename("M31.fits");
  std::string const inputfile = output_filename("M31_input.fits");

  std::string const kernel = "kb";
  t_real const over_sample = 5.;
  t_int const J = 24;
  t_real const m_over_n = std::stod(static_cast<std::string>(args[1]));
  std::string const test_number = static_cast<std::string>(args[2]);

  std::string const vis_file = output_filename("M31_vis_" + test_number + ".vis");

  auto M31 = pfitsio::read2d(fitsfile);
  t_real const max = M31.array().abs().maxCoeff();
  M31 = M31 * 1. / max;
  pfitsio::write2d(M31.real(), inputfile);
  // Following same formula in matlab example
  t_real const sigma_m = constant::pi / 3;
  // t_int const number_of_vis = std::floor(p * rho * M31.size());
  t_int const number_of_vis = std::floor(m_over_n * M31.size());
  // Generating random uv(w) coverage
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";

  PURIFY_HIGH_LOG("Number of measurements: {}", uv_data.u.size());
  // uv_data = utilities::uv_symmetry(uv_data); //reflect uv measurements
  MeasurementOperator measurements(uv_data, J, J, kernel, M31.cols(), M31.rows(), 20, over_sample);
  uv_data.vis = measurements.degrid(M31);
  utilities::write_visibility(uv_data, vis_file);
}
