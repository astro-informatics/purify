#include <iomanip>
#include <sopt/mpi/Communicator.h>
#include "catch.hpp"
#include "purify/MeasurementOperator.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/utilities.h"
using namespace purify;

TEST_CASE("Serial vs Parallel") {
  auto const world = sopt::mpi::Communicator::World();
  auto const split_comm = world.split(world.is_root());
  if(world.size() < 2) {
    std::cout << "Number of worlds: " << world.size() << std::endl;
    return;
  }

  auto const N = 5;
  auto uv_serial = utilities::uv_symmetry(utilities::random_sample_density(N, 0, constant::pi / 3));
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.vis = world.broadcast(uv_serial.vis);
  uv_serial.weights = world.broadcast(uv_serial.weights);

  utilities::vis_params uv_vis = uv_serial;
  if(split_comm.is_root() and split_comm.size() > 1) {
    auto const order
        = distribute::distribute_measurements(uv_serial, split_comm, "distance_distribution");
    uv_vis = utilities::regroup_and_scatter(uv_serial, order, split_comm);
  } else if(split_comm.size() > 1)
    uv_vis = utilities::scatter_visibilities(split_comm);

  t_int const nvis = world.broadcast(uv_vis.u.size()) == split_comm.all_sum_all(uv_vis.u.size());
  REQUIRE(world.all_reduce(nvis, MPI_BAND) != 0);

  auto const cellsize = 0.3;
  auto const over_sample = 1.375;
  auto const J = 4;
  auto const kernel = "kb_interp";
  auto const width = 10;
  auto const height = 10;
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, 5, over_sample, cellsize, cellsize);
  op.norm = world.broadcast(op.norm);

  SECTION("Degridding") {
    auto const image = world.broadcast<Image<t_complex>>(Image<t_complex>::Random(10, 10));
    auto const degridded = op.degrid(image);
    REQUIRE(degridded.size() == uv_vis.vis.size());

    if(world.is_root())
      world.broadcast(degridded);
    else if(split_comm.is_root() and split_comm.size() > 1) {
      uv_serial.vis = world.broadcast<Vector<t_complex>>();
      auto const order
          = distribute::distribute_measurements(uv_serial, split_comm, "distance_distribution");
      auto const from_serial = utilities::regroup_and_scatter(uv_serial, order, split_comm);
      CHECK(from_serial.vis.isApprox(degridded));
    } else if(split_comm.size() > 1) {
      world.broadcast<Vector<t_complex>>(); // no point to point wrappers
      auto const from_serial = utilities::scatter_visibilities(split_comm);
      CHECK(from_serial.vis.isApprox(degridded));
    }
  }

  SECTION("Gridding") {
    auto const grid = split_comm.all_sum_all(op.grid(uv_vis.vis));
    auto const serial = world.broadcast(grid);
    CHECK(grid.isApprox(serial));
  }
}
