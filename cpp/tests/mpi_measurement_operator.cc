#include <iomanip>
#include <sopt/mpi/communicator.h>
#include "catch.hpp"
#include "purify/MeasurementOperator_mpi.h"
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
  auto uv_serial = utilities::uv_symmetry(utilities::random_sample_density(N, 0, constant::pi /
  3));
  uv_serial.units = "radians";
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.vis =
  world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
  world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

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
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, 5, over_sample, cellsize,
  cellsize);
  op.norm = world.broadcast(op.norm);

  SECTION("Degridding") {
    auto const image = world.broadcast<Image<t_complex>>(Image<t_complex>::Random(width,
    height));
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

TEST_CASE("Serial vs Split FT") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = "radians";
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights
      = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if(world.is_root()) {
    auto const order
        = distribute::distribute_measurements(uv_serial, world, "distance_distribution");
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const cellsize = 1;
  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = "kb";
  auto const width = 10;
  auto const height = 10;
  auto const power_iters = 10;
  MeasurementOperator op_serial(uv_serial, J, J, kernel, width, height, power_iters, over_sample,
                                cellsize, cellsize);
  mpi::MeasurementOperator op(world, uv_mpi, J, J, kernel, width, height, power_iters, over_sample,
                              cellsize, cellsize);
  op.norm = world.broadcast(op_serial.norm);

  SECTION("Gridding") {
    auto const gridded = op.grid(uv_mpi.vis);
    auto const gridded_serial = op_serial.grid(uv_serial.vis);
    CHECK(gridded.isApprox(gridded_serial));
  }

  SECTION("Degridding") {
    auto const image = world.broadcast<Image<t_complex>>(Image<t_complex>::Random(width, height));

    auto uv_degrid = uv_serial;
    if(world.is_root()) {
      uv_degrid.vis = op_serial.degrid(image);
      auto const order
          = distribute::distribute_measurements(uv_degrid, world, "distance_distribution");
      uv_degrid = utilities::regroup_and_scatter(uv_degrid, order, world);
    } else
      uv_degrid = utilities::scatter_visibilities(world);

    Vector<t_complex> const degridded = op.degrid(image);
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    CHECK(degridded.isApprox(uv_degrid.vis));
  }
}
