#include <iomanip>
#include <random>
#include "catch.hpp"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/spherical_resample.h"
#include "purify/utilities.h"
#include <sopt/mpi/communicator.h>
#include <sopt/power_method.h>
using namespace purify;

TEST_CASE("Serial vs Distributed Operator") {
  auto const world = sopt::mpi::Communicator::World();

  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, 10);
  uv_serial.u = world.broadcast(uv_serial.u);
  uv_serial.v = world.broadcast(uv_serial.v);
  uv_serial.w = world.broadcast(uv_serial.w) * 0.;
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

  uv_mpi.units = utilities::vis_units::lambda;
  uv_serial.units = utilities::vis_units::lambda;
  const t_real L = 2.;
  const t_real theta_0 = 0.;
  const t_real phi_0 = constant::pi / 2;
  const t_int num_theta = 256;
  const t_int num_phi = 128;
  const t_int width = num_theta;
  const t_int height = num_phi;
  const t_int number_of_samples = num_theta * num_phi;
  const t_int Jl = 4;
  const t_int Jm = 4;
  const t_int Ju = 4;
  const t_int Jv = 4;
  const t_int Jw = 512;
  const t_real oversample_ratio_image_domain = 2;
  const t_real oversample_ratio = 2;
  const kernels::kernel kernel = kernels::kernel::kb;
  const operators::fftw_plan ft_plan = operators::fftw_plan::measure;
  const auto phi = [num_phi](const t_int k) -> t_real {
    return utilities::ind2row(k, num_phi, num_theta) * constant::pi / num_phi;
  };
  const auto theta = [num_theta, num_phi](const t_int k) -> t_real {
    return utilities::ind2col(k, num_phi, num_theta) * 2 * constant::pi / num_theta;
  };

  const Vector<t_complex> power_init =
      world.broadcast(Vector<t_complex>::Random(height * width).eval());
  const auto op_serial = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::spherical_resample::measurement_operator::nonplanar_degrid_wproj_operator<
          Vector<t_complex>, std::function<t_real(t_int)>>(
          number_of_samples, theta_0, phi_0, theta, phi, uv_serial, oversample_ratio,
          oversample_ratio_image_domain, kernel, Ju, Jw, Jl, Jm, ft_plan, false, L, 1e-6, 1e-6),
      100, 1e-6, power_init));

  const auto op = std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
      purify::spherical_resample::measurement_operator::nonplanar_degrid_wproj_operator<
          Vector<t_complex>, std::function<t_real(t_int)>>(
          world, number_of_samples, theta_0, phi_0, theta, phi, uv_mpi, oversample_ratio,
          oversample_ratio_image_domain, kernel, Ju, Jw, Jl, Jm, ft_plan, false, L, 1e-6, 1e-6),
      100, 1e-6, power_init));

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
    CAPTURE(degridded.head(5));
    CAPTURE(uv_degrid.vis.head(5));
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
