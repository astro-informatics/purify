#include <catch.hpp>
#include <numeric>
#include <random>
#include <utility>

#include <sopt/imaging_padmm.h>
#include <sopt/logging.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/utilities.h>
#include <sopt/wavelets.h>
#include "purify/MeasurementOperator.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

using namespace purify;
utilities::vis_params dirty_visibilities(t_uint number_of_vis = 10, t_uint width = 20,
                                         t_uint height = 20, t_uint over_sample = 2,
                                         t_real ISNR = 30) {
  auto result = utilities::random_sample_density(number_of_vis, 0, constant::pi / 3);
  result.units = "radians";
  result.vis = Vector<t_complex>::Random(result.u.size());
  result.weights = Vector<t_complex>::Random(result.u.size());
  return result;
}

utilities::vis_params dirty_visibilities(sopt::mpi::Communicator const &comm,
                                         t_uint number_of_vis = 10, t_uint width = 20,
                                         t_uint height = 20, t_uint over_sample = 2,
                                         t_real ISNR = 30) {
  utilities::vis_params result;
  if(comm.is_root())
    result = dirty_visibilities(number_of_vis, width, height, over_sample, ISNR);
  result.u = comm.broadcast(result.u);
  result.v = comm.broadcast(result.v);
  result.w = comm.broadcast(result.w);
  result.weights = comm.broadcast(result.weights);
  result.vis = comm.broadcast(result.vis);
  result.ra = comm.broadcast(result.ra);
  result.dec = comm.broadcast(result.dec);
  result.average_frequency = comm.broadcast(result.average_frequency);
  result.units = comm.broadcast(result.units);
  return result;
}

TEST_CASE("Serial vs. Parallel PADMM with random coverage.") {
  logging::set_level("debug");
  sopt::logging::set_level("debug");

  auto const world = sopt::mpi::Communicator::World();
  // split into serial and parallel
  auto const split_comm = world.split(world.is_root());
  if(world.size() < 2)
    return;

  auto const nvis = 10;
  auto const width = 12;
  auto const height = 12;
  auto const kernel = "kb";
  auto const over_sample = 2;
  auto const J = 1;
  auto const ISNR = 30;

  auto const uv_serial = dirty_visibilities(world, nvis, width, height, over_sample, ISNR);
  // distribute only on processors doing it parallel
  auto const uv_data = distribute_params(uv_serial, split_comm);

  auto measurements = std::make_shared<MeasurementOperator>(uv_data, J, J, kernel, width, height,
                                                            100, over_sample);
  measurements->norm = world.broadcast(measurements->norm);
  auto const Phi = linear_transform(measurements, uv_data.vis.size(), split_comm);

  SECTION("Measurement operator parallelization") {
    SECTION("Gridding") {
      Vector<t_complex> const grid = Phi.adjoint() * uv_data.vis;
      auto const serial = world.broadcast(grid);
      CAPTURE(split_comm.is_root());
      CHECK(grid.isApprox(serial));
    }

    SECTION("Degridding") {
      auto const image
          = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(width * height));
      Vector<t_complex> const degridded = Phi * image;
      REQUIRE(degridded.size() == uv_data.vis.size());

      auto const just_roots = world.split(split_comm.is_root());
      if(world.is_root())
        just_roots.broadcast(degridded);
      else if(split_comm.is_root() and split_comm.size() > 1) {
        utilities::vis_params serial = uv_serial;
        serial.vis = just_roots.broadcast<Vector<t_complex>>();
        auto const order
            = distribute::distribute_measurements(serial, split_comm, "distance_distribution");
        auto const from_serial = utilities::regroup_and_scatter(serial, order, split_comm);
        CHECK(from_serial.vis.isApprox(degridded));
      } else if(split_comm.size() > 1) {
        auto const from_serial = utilities::scatter_visibilities(split_comm);
        CHECK(from_serial.vis.isApprox(degridded));
      }
    }
  }

  sopt::wavelets::SARA const sara(
      {std::make_tuple("DB4", 2u), std::make_tuple("DB2", 2u), std::make_tuple("DB1", 2u)});
  auto const start = [](t_uint size, t_uint ncomms, t_uint rank) {
    return std::min(size, rank * (size / ncomms) + std::min(rank, size % ncomms));
  };
  auto const startw = start(sara.size(), split_comm.size(), split_comm.rank());
  auto const endw = start(sara.size(), split_comm.size(), split_comm.rank() + 1);
  auto const split_sara = sopt::wavelets::SARA(sara.begin() + startw, sara.begin() + endw);
  auto const Psi = sopt::linear_transform<t_complex>(split_sara, measurements->imsizey(),
                                                     measurements->imsizex(), split_comm);
  SECTION("Wavelet operator parallelization") {
    auto const Nx = measurements->imsizex();
    auto const Ny = measurements->imsizey();
    SECTION("Signal to Coefficients") {
      auto const signal = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(Nx * Ny));
      Vector<t_complex> const coefficients = Psi.adjoint() * signal;
      CHECK(world.broadcast(coefficients)
                .segment(startw * Nx * Ny, (endw - startw) * Nx * Ny)
                .isApprox(coefficients));
    }

    SECTION("Coefficients to Signal") {
      auto const coefficients
          = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(Nx * Ny * sara.size()));
      Vector<t_complex> const signal
          = Psi * coefficients.segment(startw * Nx * Ny, (endw - startw) * Nx * Ny);
      CHECK(world.broadcast(signal).isApprox(signal));
    }
  }

  Vector<> dimage = (Phi.adjoint() * uv_data.vis).real();
  t_real const max_val = dimage.array().abs().maxCoeff();
  dimage = dimage / max_val;
  Vector<t_complex> initial_estimate = Vector<t_complex>::Zero(dimage.size());

  auto const sigma = world.broadcast(utilities::SNR_to_standard_deviation(uv_data.vis, ISNR));
  auto const epsilon = world.broadcast(utilities::calculate_l2_radius(uv_data.vis, sigma));
  auto const purify_gamma
      = world.is_root() ?
            world.broadcast((Psi.adjoint() * (Phi.adjoint() * uv_data.vis)).cwiseAbs().maxCoeff()
                            * 1e-3) :
            world.broadcast<t_real>();
  PURIFY_HIGH_LOG("Starting sopt!");
  PURIFY_MEDIUM_LOG("Epsilon {}", epsilon);
  PURIFY_MEDIUM_LOG("Gamma {}", purify_gamma);
  auto padmm = sopt::algorithm::ImagingProximalADMM<t_complex>(uv_data.vis)
                   .gamma(purify_gamma)
                   .relative_variation(1e-3)
                   // communicator ensuring l2 norm in l2ball proximal is global
                   .l2ball_proximal_communicator(split_comm)
                   // communicator ensuring l1 norm in l1 proximal is global
                   .l1_proximal_adjoint_space_comm(world)
                   .tight_frame(false)
                   .l1_proximal_tolerance(1e-2)
                   .l1_proximal_nu(1)
                   .l1_proximal_itermax(50)
                   .l1_proximal_positivity_constraint(true)
                   .l1_proximal_real_constraint(true)
                   .lagrange_update_scale(0.9)
                   .nu(1e0)
                   .Psi(Psi)
                   .Phi(Phi)
                   .itermax(5);
  padmm.residual_convergence([&padmm, epsilon, split_comm,
                              world](Vector<t_complex> const &, Vector<t_complex> const &residual) {
    auto const tolerance = epsilon * 1.001;
    auto const residual_norm
        = sopt::mpi::l2_norm(residual, padmm.l2ball_proximal_weights(), split_comm);
    SOPT_LOW_LOG("    - Residuals: epsilon = {}, residual norm = {}", tolerance, residual_norm);
    auto const result = residual_norm < tolerance;
    CHECK(result == (world.broadcast<int>(result, world.root_id()) != 0));
    return result;
  });
  sopt::ScalarRelativeVariation<t_complex> conv(padmm.relative_variation(),
                                                padmm.relative_variation(), "Objective function");
  padmm.objective_convergence(
      [&padmm, conv, split_comm, world](Vector<t_complex> const &,
                                        Vector<t_complex> const &residual) mutable -> bool {
        auto const result = conv(
            sopt::mpi::l1_norm(residual + padmm.target(), padmm.l1_proximal_weights(), split_comm));
        REQUIRE(result == (world.broadcast<int>(result, world.root_id()) != 0));
        return result;
      });
  // the following is transformed to a function of the image and the residuals automatically
  padmm.is_converged([world](Vector<t_complex> const &image) -> bool {
    auto const from_root = world.broadcast(image);
    REQUIRE(from_root.isApprox(image, 1e-12));
    return true;
  });

  SECTION("Check parameters") {
    CHECK(world.broadcast(padmm.gamma()) == Approx(padmm.gamma()));
    CHECK(world.broadcast(padmm.relative_variation()) == Approx(padmm.relative_variation()));
    CHECK(world.broadcast(padmm.l2ball_proximal_epsilon())
          == Approx(padmm.l2ball_proximal_epsilon()));
  }

  SECTION("Run serial vs parallel") {
    auto const diagnostic = padmm();
    CHECK(diagnostic.good == (world.broadcast<int>(diagnostic.good, world.root_id()) != 0));
    CHECK(diagnostic.x.isApprox(world.broadcast(diagnostic.x)));
  }
}
