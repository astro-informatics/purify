#include "purify/config.h"
#include "purify/types.h"
#include <ctime>
#include "purify/directories.h"
#include "purify/logging.h"
#include "purify/measurement_operator_factory.h"
#include "purify/utilities.h"

using namespace purify;
using namespace purify::notinstalled;

int main(int nargs, char const **args) {
  auto const session = sopt::mpi::init(nargs, args);
  auto const world = sopt::mpi::Communicator::World();
  purify::logging::initialize();
  purify::logging::set_level("debug");
  // Gridding example
  auto const oversample_ratio = 2;
  auto const Ju = 4;
  auto const Jw = 100;
  const t_real FoV = 30 * 60 * 60;
  const t_real k_means_rel_diff = 1e-5;
  std::string const results = output_filename("mems_" + std::to_string(world.size()) + ".txt");
  std::ofstream out(world.is_root() ? results : output_filename("empty.txt"));
  if (world.is_root()) {
    out.precision(13);
  }
  t_int const number_of_vis = std::pow(10, 6);
  const t_real rms_w = 200;  // lambda
  t_int const imsizex = 1024;
  t_int const imsizey = imsizex;
  const t_real cell = FoV / static_cast<t_real>(imsizex);
  auto const kernel = "kb";
  const t_int channels = 80;

  const std::string pos_filename = mwa_filename("Phase1_config.txt");
  t_uint const number_of_pixels = imsizex * imsizey;
  // Generating random uv(w) coverage
  t_real const sigma_m = constant::pi / 3;
  Vector<t_real> mem_stack = Vector<t_real>::Zero(world.size());
  Vector<t_real> mem_node = Vector<t_real>::Zero(world.size());
  std::vector<t_real> frequencies;
  for (t_int k = 0; k < channels; k++)
    frequencies.push_back(87.68e6 +
                          (world.rank() * channels + k - world.size() * channels * 0.5) * 40e3);

  const std::vector<t_real> times = {0.,  8.,  16., 24., 32., 40.,  48., 56.,
                                     64., 72., 80., 88., 96., 104., 112.};
  const t_real theta_ra = 0. * 180. / constant::pi;
  const t_real phi_dec = 18.3 * 180. / constant::pi;
  const t_real latitude = -26.7 * 180. / constant::pi;
  auto uv_data = utilities::read_ant_positions_to_coverage(pos_filename, frequencies, times,
                                                           theta_ra, phi_dec, latitude);
  const t_real number_of_baselines = uv_data.size() / times.size() / channels;
  uv_data = utilities::conjugate_w(uv_data);
  const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
  const t_real du = widefield::pixel_to_lambda(cell, imsizex, oversample_ratio);
  std::vector<t_real> w_stacks;
  std::vector<t_int> image_index;
  std::tie(uv_data, image_index, w_stacks) = utilities::w_stacking_with_all_to_all(
      uv_data, du, Ju, Jw, world, 100, 0., cost, k_means_rel_diff);
  for (t_real k = 0; k < uv_data.size(); k++) {
    const t_int Ju_max = widefield::w_support(uv_data.w(k) - w_stacks.at(image_index.at(k)), du,
                                              static_cast<t_int>(Ju), static_cast<t_int>(Jw));
    const t_real mem = (Ju_max * Ju_max) * 16.;
    mem_stack(image_index.at(k)) += mem;
    mem_node(world.rank()) += mem;
  }
  world.all_sum_all<Vector<t_real>>(mem_stack);
  world.all_sum_all<Vector<t_real>>(mem_node);
  if (world.is_root()) {
    out << number_of_vis << " " << rms_w << std::endl;  // in seconds
    out << "stacks" << std::endl;                       // in seconds
    out << mem_stack.transpose() / mem_stack.sum() << std::endl;
    out << "nodes" << std::endl;  // in seconds
    out << mem_node.transpose() / mem_node.sum() << std::endl;
  }
  out.close();
}
