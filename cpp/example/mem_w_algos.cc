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
  const t_int iters = (nargs > 1) ? std::stod(static_cast<std::string>(args[1])) : 1;
  // Gridding example
  auto const oversample_ratio = 2;
  auto const Ju = 4;
  auto const Jw = 100;
  const t_real FoV = 25 * 60 * 60;
  const t_real k_means_rel_diff = 1e-5;
  std::string const results = output_filename("mems_" + std::to_string(world.size()) + ".txt");
  std::ofstream out(world.is_root() ? results : output_filename("empty.txt"));
  if (world.is_root()) {
    out.precision(13);
  }
  for (t_int nvis_iters = 6; nvis_iters < 9; nvis_iters++) {
    for (t_int w_iter = 1; w_iter < 4; w_iter++) {
      t_int const number_of_vis = std::pow(10, nvis_iters);
      const t_real rms_w = 50. * w_iter;  // lambda
      t_int const imsizex = 1024;
      t_int const imsizey = imsizex;
      const t_real cell = FoV / static_cast<t_real>(imsizex);
      auto const kernel = "kb";

      t_uint const number_of_pixels = imsizex * imsizey;
      // Generating random uv(w) coverage
      t_real const sigma_m = constant::pi / 3;
      Vector<t_real> mem_stack = Vector<t_real>::Zero(world.size());
      Vector<t_real> mem_node = Vector<t_real>::Zero(world.size());
      for (t_int i = 0; i < iters; i++) {
        auto uv_data = utilities::random_sample_density(std::floor(number_of_vis / world.size()), 0,
                                                        sigma_m, rms_w);
        uv_data = utilities::conjugate_w(uv_data);
        const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
        const t_real du = widefield::pixel_to_lambda(cell, imsizex, oversample_ratio);
        std::vector<t_real> w_stacks;
        std::vector<t_int> image_index;
        std::tie(uv_data, image_index, w_stacks) = utilities::w_stacking_with_all_to_all(
            uv_data, du, Ju, Jw, world, 100, 0., cost, k_means_rel_diff);
        for (t_real k = 0; k < uv_data.size(); k++) {
          const t_int Ju_max =
              widefield::w_support(uv_data.w(k) - w_stacks.at(image_index.at(k)), du,
                                   static_cast<t_int>(Ju), static_cast<t_int>(Jw));
          const t_real mem = (Ju_max * Ju_max) * 16.;
          mem_stack(image_index.at(k)) += mem * 1e-6 / iters;
          mem_node(world.rank()) += mem * 1e-6 / iters;
        }
      }
      world.all_sum_all<Vector<t_real>>(mem_stack);  // in megabytes
      world.all_sum_all<Vector<t_real>>(mem_node);   // in megabytes
      if (world.is_root()) {
        out << number_of_vis << " " << rms_w << std::endl;  // in seconds
        out << "stacks" << std::endl;                       // in seconds
        out << mem_stack.transpose() << std::endl;
        out << "nodes" << std::endl;  // in seconds
        out << mem_node.transpose() << std::endl;
      }
    }
  }
  out.close();
}
