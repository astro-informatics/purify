#include <catch.hpp>
#include <numeric>
#include <random>
#include <utility>

#include <sopt/imaging_padmm.h>
#include <sopt/logging.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/utilities.h>
#include <sopt/wavelets.h>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/types.h"
#include "purify/utilities.h"

#include "purify/algorithm_factory.h"
#include "purify/wavelet_operator_factory.h"
#include "purify/measurement_operator_factory.h"

using namespace purify;
utilities::vis_params dirty_visibilities(const std::vector<std::string> &names) {
  return utilities::read_visibility(names, false);
}

utilities::vis_params
dirty_visibilities(const std::vector<std::string> &names, sopt::mpi::Communicator const &comm) {
  if(comm.size() == 1)
    return dirty_visibilities(names);
  if(comm.is_root()) {
    auto result = dirty_visibilities(names);
    auto const order = distribute::distribute_measurements(result, comm, distribute::plan::w_term);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
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

  auto const nvis = 20;
  auto const width = 32;
  auto const height = 32;
  auto const over_sample = 2;
  auto const J = 4;
  auto const ISNR = 30;

  auto const uv_serial = dirty_visibilities(world, nvis, width, height, over_sample, ISNR);
  // distribute only on processors doing it parallel
  auto const uv_data = distribute_params(uv_serial, split_comm);



 auto   Phi
      =  *measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
          split_comm, uv_data.u, uv_data.v, uv_data.w, uv_data.weights, width, height, over_sample, 500, 1e-9);

  SECTION("Measurement operator parallelization") {
    SECTION("Gridding") {
      Vector<t_complex> const grid = Phi.adjoint() * uv_data.vis;
      auto const serial = world.broadcast(grid);
      CAPTURE(split_comm.is_root());
      CAPTURE((grid.array()/serial.array()).head(5));
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
            = distribute::distribute_measurements(serial, split_comm, distribute::plan::radial);
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
  auto const Psi = sopt::linear_transform<t_complex>(split_sara, height,
                                                     width, split_comm);
  SECTION("Wavelet operator parallelization") {
    auto const Nx = width;
    auto const Ny = height;
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
  auto padmm = factory::algorithm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(factory::algorithm::padmm, 
      factory::algo_distribution::mpi_serial, 
      std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(Psi), 
      std::make_shared<sopt::LinearTransform<Vector<t_complex>>>(Phi), uv_data, sigma, height, width);
}
