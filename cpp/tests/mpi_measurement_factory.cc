#include <sopt/mpi/communicator.h>
#include "catch.hpp"
#include "purify/measurement_operator_factory.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/utilities.h"
using namespace purify;

TEST_CASE("Serial vs Distributed Operator") {
  purify::logging::set_level("debug");
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights
      = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if(world.is_root()) {
    auto const order
        = distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100);
  CAPTURE(world.size());

  for(auto method :{factory::distributed_measurement_operator::mpi_distribute_image, factory::distributed_measurement_operator::mpi_distribute_grid}){
  const auto op = factory::measurement_operator_factory<Vector<t_complex>>(
      method, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample, 100);
  if(uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image
        = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if(world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order
          = distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
      uv_degrid = utilities::regroup_and_scatter(uv_degrid, order, world);
    } else
      uv_degrid = utilities::scatter_visibilities(world);
    Vector<t_complex> const degridded = *op * image;
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    REQUIRE(degridded.isApprox(uv_degrid.vis, 1e-4));
  }
  SECTION("Gridding") {
    Vector<t_complex> const gridded = op->adjoint() * uv_mpi.vis;
    Vector<t_complex> const gridded_serial = op_serial->adjoint() * uv_serial.vis;
    REQUIRE(gridded.size() == gridded_serial.size());
    REQUIRE(gridded.isApprox(gridded_serial, 1e-4));
  }
  }
}
TEST_CASE("GPU Serial vs Distributed Operator") {
  purify::logging::set_level("debug");
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights
      = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if(world.is_root()) {
    auto const order
        = distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100);
  CAPTURE(world.size());

  for(auto method :{factory::distributed_measurement_operator::gpu_mpi_distribute_image, factory::distributed_measurement_operator::gpu_mpi_distribute_grid}){
#ifndef PURIFY_ARRAYFIRE
  REQUIRE_THROWS(factory::measurement_operator_factory<Vector<t_complex>>(
      method, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample, 100));
#else
  const auto op = factory::measurement_operator_factory<Vector<t_complex>>(
      method, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample, 100);

  if(uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image
        = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if(world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order
          = distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
      uv_degrid = utilities::regroup_and_scatter(uv_degrid, order, world);
    } else
      uv_degrid = utilities::scatter_visibilities(world);
    Vector<t_complex> const degridded = *op * image;
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    REQUIRE(degridded.isApprox(uv_degrid.vis, 1e-4));
  }
  SECTION("Gridding") {
    Vector<t_complex> const gridded = op->adjoint() * uv_mpi.vis;
    Vector<t_complex> const gridded_serial = op_serial->adjoint() * uv_serial.vis;
    REQUIRE(gridded.size() == gridded_serial.size());
    REQUIRE(gridded.isApprox(gridded_serial, 1e-4));
  }
#endif
  }
}
