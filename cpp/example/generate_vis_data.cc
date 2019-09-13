#include "purify/config.h"
#include "purify/types.h"
#include <array>
#include <memory>
#include <random>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/operators.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>

int main(int nargs, char const **args) {
  using namespace purify;
  using namespace purify::notinstalled;
  purify::logging::initialize();
  purify::logging::set_level(purify::default_logging_level());

  const std::string &pos_filename = mwa_filename("Phase1_config.txt");
  const std::string &vis_file = output_filename("mwa_snapshot_coverage.vis");
  const auto times = std::vector<t_real> { 0, 8, 16, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112 };
  auto uv_data = utilities::read_ant_positions_to_coverage(pos_filename, 150e6, times, 0., 0., 0.);
  uv_data.units = utilities::vis_units::lambda;

  PURIFY_HIGH_LOG("Number of measurements: {}", uv_data.u.size());
  utilities::write_visibility(uv_data, vis_file, true);
}
