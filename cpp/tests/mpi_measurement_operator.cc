#include <iomanip>
#include "catch.hpp"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/operators.h"
#include "purify/utilities.h"
#include "purify/wproj_operators.h"
#include <sopt/mpi/communicator.h>
#include <sopt/power_method.h>
#ifdef PURIFY_ARRAYFIRE
#include "purify/operators_gpu.h"
#include "purify/wproj_operators_gpu.h"
#endif
using namespace purify;

TEST_CASE("Serial vs Distributed Operator") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample),
      100, 1e-4, Vector<t_complex>::Random(height * width)));

  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          world, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample),
      100, 1e-4, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
TEST_CASE("Serial vs Distributed Fourier Grid Operator") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample),
      100, 1e-4, Vector<t_complex>::Random(height * width)));

  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          world, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample),
      100, 1e-4, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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

TEST_CASE("Serial vs Distributed Fourier Grid Operator weighted") {
  // sopt::logging::set_level("debug");
  // purify::logging::set_level("debug");
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample),
      100, 1e-4, Vector<t_complex>::Random(height * width)));
  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d_mpi<Vector<t_complex>>(
          world, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample),
      100, 1e-4, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (world.size() == 1) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
#ifdef PURIFY_ARRAYFIRE
TEST_CASE("Serial vs Distributed GPU Fourier Grid Operator weighted") {
  // sopt::logging::set_level("debug");
  // purify::logging::set_level("debug");
  af::setDevice(0);
  auto const world = sopt::mpi::Communicator::World();
  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample),
      100, 1e-4, Vector<t_complex>::Random(height * width)));
  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::gpu::measurementoperator::init_degrid_operator_2d_mpi(
          world, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample),
      100, 1e-4, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (world.size() == 1) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Power Method") {
    auto op_norm = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        *op, 100, 1e-4, Vector<t_complex>::Random(width * height)));
    CHECK(std::abs(op_norm - 1.) < 1e-4);
    auto op_norm_old = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        *op_serial, 100, 1e-4, Vector<t_complex>::Random(width * height)));
    CHECK(std::abs(op_norm_old - 1.) < 1e-4);
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
TEST_CASE("Serial vs Distributed GPU Operator weighted") {
  // sopt::logging::set_level("debug");
  // purify::logging::set_level("debug");
  af::setDevice(0);
  auto const world = sopt::mpi::Communicator::World();
  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w);
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample),
      100, 1e-4, Vector<t_complex>::Random(height * width)));
  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::gpu::measurementoperator::init_degrid_operator_2d(
          world, uv_mpi.u, uv_mpi.v, uv_mpi.w, uv_mpi.weights, height, width, over_sample),
      100, 1e-4, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (world.size() == 1) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Power Method") {
    auto op_norm = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        *op, 100, 1e-4, Vector<t_complex>::Random(width * height)));
    CHECK(std::abs(op_norm - 1.) < 1e-4);
    auto op_norm_old = std::get<0>(sopt::algorithm::power_method<Vector<t_complex>>(
        *op_serial, 100, 1e-4, Vector<t_complex>::Random(width * height)));
    CHECK(std::abs(op_norm_old - 1.) < 1e-4);
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
TEST_CASE("Serial vs Distributed GPU Operator Radial WProjection") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(Vector<t_real>::Zero(uv_serial.size()).eval());
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const t_real cellx = 1;
  const t_real celly = 1;
  const t_real abs_error = 1e-6;
  const t_real rel_error = 1e-8;
  const Vector<t_complex> power_init =
      world.broadcast(Vector<t_complex>::Random(height * width).eval());
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial, height, width, cellx, celly, over_sample, kernel, J, 4, true, abs_error,
          rel_error, dde_type::wkernel_radial),
      10000, 1e-5, power_init);

  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      gpu::measurementoperator::init_degrid_operator_2d(world, uv_mpi, height, width, cellx, celly,
                                                        over_sample, kernel, J, 4, true, abs_error,
                                                        rel_error, dde_type::wkernel_radial),
      10000, 1e-5, power_init);

  if (uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
#endif

TEST_CASE("Serial vs Distributed Operator Radial WProjection") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 1000;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(Vector<t_real>::Zero(uv_serial.size()).eval());
  uv_serial.units = utilities::vis_units::radians;
  uv_serial.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));
  uv_serial.weights =
      world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(uv_serial.u.size()));

  utilities::vis_params uv_mpi;
  if (world.is_root()) {
    auto const order =
        distribute::distribute_measurements(uv_serial, world, distribute::plan::radial);
    uv_mpi = utilities::regroup_and_scatter(uv_serial, order, world);
  } else
    uv_mpi = utilities::scatter_visibilities(world);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const t_real cellx = 1;
  const t_real celly = 1;
  const t_real abs_error = 1e-8;
  const t_real rel_error = 1e-8;
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          uv_serial, height, width, cellx, celly, over_sample, kernel, J, 4, true, abs_error,
          rel_error, dde_type::wkernel_radial),
      10000, 1e-5, Vector<t_complex>::Random(height * width)));

  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          world, uv_mpi, height, width, cellx, celly, over_sample, kernel, J, 4, true, abs_error,
          rel_error, dde_type::wkernel_radial),
      10000, 1e-5, world.broadcast(Vector<t_complex>::Random(height * width).eval())));

  if (uv_serial.u.size() == uv_mpi.u.size()) {
    REQUIRE(uv_serial.u.isApprox(uv_mpi.u));
    CHECK(uv_serial.v.isApprox(uv_mpi.v));
    CHECK(uv_serial.weights.isApprox(uv_mpi.weights));
  }
  SECTION("Degridding") {
    Vector<t_complex> const image =
        world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));

    auto uv_degrid = uv_serial;
    if (world.is_root()) {
      uv_degrid.vis = *op_serial * image;
      auto const order =
          distribute::distribute_measurements(uv_degrid, world, distribute::plan::radial);
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
