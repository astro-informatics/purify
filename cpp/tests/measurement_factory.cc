#include "catch.hpp"
#include "purify/logging.h"
#include "purify/measurement_operator_factory.h"
#include "purify/utilities.h"
#include <sopt/power_method.h>
using namespace purify;

TEST_CASE("Serial vs Distributed Operator") {
  purify::logging::set_level("debug");

  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample);
  const auto op = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::serial, uv_serial.u, uv_serial.v, uv_serial.w,
      uv_serial.weights, height, width, over_sample);

  SECTION("Degridding") {
    Vector<t_complex> const image = Vector<t_complex>::Random(width * height);

    auto uv_degrid = uv_serial;
    uv_degrid.vis = *op_serial * image;
    Vector<t_complex> const degridded = *op * image;
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    REQUIRE(degridded.isApprox(uv_degrid.vis, 1e-4));
  }
  SECTION("Gridding") {
    Vector<t_complex> const gridded = op->adjoint() * uv_serial.vis;
    Vector<t_complex> const gridded_serial = op_serial->adjoint() * uv_serial.vis;
    REQUIRE(gridded.size() == gridded_serial.size());
    REQUIRE(gridded.isApprox(gridded_serial, 1e-4));
  }
}

TEST_CASE("GPU Serial vs Distributed Operator") {
  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);

  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample);
#ifndef PURIFY_ARRAYFIRE
  REQUIRE_THROWS(factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::gpu_serial, uv_serial.u, uv_serial.v, uv_serial.w,
      uv_serial.weights, height, width, over_sample));
#else
  const auto op = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::gpu_serial, uv_serial.u, uv_serial.v, uv_serial.w,
      uv_serial.weights, height, width, over_sample);

  SECTION("Degridding") {
    Vector<t_complex> const image = Vector<t_complex>::Random(width * height);

    auto uv_degrid = uv_serial;
    uv_degrid.vis = *op_serial * image;
    Vector<t_complex> const degridded = *op * image;
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    REQUIRE(degridded.isApprox(uv_degrid.vis, 1e-4));
  }
  SECTION("Gridding") {
    Vector<t_complex> const gridded = op->adjoint() * uv_serial.vis;
    Vector<t_complex> const gridded_serial = op_serial->adjoint() * uv_serial.vis;
    REQUIRE(gridded.size() == gridded_serial.size());
    REQUIRE(gridded.isApprox(gridded_serial, 1e-4));
  }
#endif
}
