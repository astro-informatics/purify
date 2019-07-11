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
  t_int const conj = (nargs > 1) ? std::stod(static_cast<std::string>(args[1])) : 0;
  const t_int iters = (nargs > 2) ? std::stod(static_cast<std::string>(args[2])) : 1;
  // Gridding example
  auto const oversample_ratio = 2;
  auto const Ju = 4;
  auto const Jw = 100;
  const t_real FoV = 25 * 60 * 60;
  std::string const results = output_filename("times_" + std::to_string(world.size()) +
                                              ((static_cast<bool>(conj)) ? "_conj" : "") + ".txt");
  std::ofstream out;
  if (world.is_root()) {
    out = std::ofstream(results);
    out.precision(13);
  }
  for (t_int nvis_iters = 5; nvis_iters < 8; nvis_iters++) {
    for (t_int w_iter = 1; w_iter < 4; w_iter++) {
      for (t_int cell_iters = 8; cell_iters < 11; cell_iters++) {
        t_int const number_of_vis = std::pow(10, nvis_iters);
        const t_real rms_w = 10. * w_iter;  // lambda
        t_int const imsizex = std::pow(2, cell_iters);
        t_int const imsizey = imsizex;
        const t_real cell = FoV / static_cast<t_real>(imsizex);
        auto const kernel = "kb";

        t_uint const number_of_pixels = imsizex * imsizey;
        // Generating random uv(w) coverage
        t_real const sigma_m = constant::pi / 3;
        auto uv_data = utilities::random_sample_density(std::floor(number_of_vis / world.size()), 0,
                                                        sigma_m, rms_w);
        if (static_cast<bool>(conj)) uv_data = utilities::conjugate_w(uv_data);
        const auto cost = [](t_real x) -> t_real { return std::abs(x * x); };
        uv_data = utilities::w_stacking(uv_data, world, 100, cost, 1e-3);
        uv_data.units = utilities::vis_units::radians;
        const Vector<t_complex> image = Vector<t_complex>::Random(number_of_pixels);
        t_real con_time = 0;
        t_real app_time = 0;
        for (t_int i = 0; i < iters; i++) {
          auto begin = std::chrono::high_resolution_clock::now();
          const auto measure_op = factory::measurement_operator_factory<Vector<t_complex>>(
              factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey,
              imsizex, cell, cell, oversample_ratio, kernels::kernel_from_string.at(kernel), Ju, Jw,
              true, 1e-6, 1e-6, dde_type::wkernel_radial);
          auto end = std::chrono::high_resolution_clock::now();
          con_time += world.all_reduce<t_real>(
              std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() * 1e-9,
              MPI_MAX);  // total time for construction to run in seconds
          world.barrier();
          begin = std::chrono::high_resolution_clock::now();
          Vector<t_complex> const measurements =
              measure_op->adjoint() * (*measure_op * image).eval();
          end = std::chrono::high_resolution_clock::now();
          app_time += world.all_reduce<t_real>(
              std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() * 1e-9,
              MPI_MAX);  // total time for application to run in seconds
        }
        con_time /= static_cast<t_real>(iters);
        app_time /= static_cast<t_real>(iters);
        if (world.is_root()) {
          out << number_of_vis << " " << rms_w << " " << imsizex << " " << con_time << " "
              << app_time << std::endl;  // in seconds
          PURIFY_LOW_LOG("Mean Construction Time: {}, Mean Application Time: {}", con_time,
                         app_time);
        }
      }
    }
  }
  out.close();
}
