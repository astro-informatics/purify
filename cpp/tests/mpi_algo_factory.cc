#include <catch.hpp>
#include <numeric>
#include <random>
#include <utility>

#include "purify/types.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include <sopt/imaging_padmm.h>
#include <sopt/logging.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/utilities.h>
#include <sopt/power_method.h>
#include <sopt/wavelets.h>

#include "purify/algorithm_factory.h"
#include "purify/measurement_operator_factory.h"
#include "purify/wavelet_operator_factory.h"

using namespace purify;
utilities::vis_params dirty_visibilities(const std::vector<std::string> &names) {
  return utilities::read_visibility(names, false);
}

utilities::vis_params dirty_visibilities(const std::vector<std::string> &names,
                                         sopt::mpi::Communicator const &comm) {
  if (comm.size() == 1) return dirty_visibilities(names);
  if (comm.is_root()) {
    auto result = dirty_visibilities(names);
    auto const order = distribute::distribute_measurements(result, comm, distribute::plan::w_term);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}

TEST_CASE("Serial vs. Serial with MPI PADMM") {
  auto const world = sopt::mpi::Communicator::World();

  const std::string &test_dir = "expected/padmm/";
  const std::string &input_data_path = notinstalled::data_filename(test_dir + "input_data.vis");

  auto uv_data = dirty_visibilities({input_data_path}, world);
  uv_data.units = utilities::vis_units::radians;
  if (world.is_root()) CAPTURE(uv_data.vis.head(5));
  REQUIRE(world.all_sum_all(uv_data.size()) == 13107);

  t_uint const imsizey = 256;
  t_uint const imsizex = 256;

  auto const measurements_transform =
      std::get<2>(sopt::algorithm::normalise_operator<Vector<t_complex>>(
          factory::measurement_operator_factory<Vector<t_complex>>(
              factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey,
              imsizex, 1, 1, 2, kernels::kernel_from_string.at("kb"), 4, 4),
          1000, 1e-5, world.broadcast(Vector<t_complex>::Ones(imsizex * imsizey).eval())));
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex);
  t_real const sigma = world.broadcast(0.02378738741225);  // see test_parameters file
  SECTION("global") {
    auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data, sigma,
        imsizey, imsizex, sara.size(), 500);

    auto const diagnostic = (*padmm)();
    CHECK(diagnostic.niters == 139);

    const std::string &expected_solution_path =
        notinstalled::data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path =
        notinstalled::data_filename(test_dir + "residual.fits");

    const auto solution = pfitsio::read2d(expected_solution_path);
    const auto residual = pfitsio::read2d(expected_residual_path);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-6));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-6));
  }
  SECTION("local") {
    auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        factory::algo_distribution::mpi_distributed, measurements_transform, wavelets, uv_data,
        sigma, imsizey, imsizex, sara.size(), 500);

    auto const diagnostic = (*padmm)();
    t_real const epsilon = utilities::calculate_l2_radius(world.all_sum_all(uv_data.vis.size()),
                                                          world.broadcast(sigma));
    CHECK(sopt::mpi::l2_norm(diagnostic.residual, padmm->l2ball_proximal_weights(), world) <
          epsilon);
    // the algorithm depends on nodes, so other than a basic bounds check,
    // it is hard to know exact precision (might depend on probability theory...)
    if (world.size() > 2 or world.size() == 0) return;
    // testing the case where there are two nodes exactly.
    const std::string &expected_solution_path =
        (world.size() == 2) ? notinstalled::data_filename(test_dir + "mpi_solution.fits")
                            : notinstalled::data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path =
        (world.size() == 2) ? notinstalled::data_filename(test_dir + "mpi_residual.fits")
                            : notinstalled::data_filename(test_dir + "residual.fits");
    if (world.size() == 1) CHECK(diagnostic.niters == 139);
    if (world.size() == 2) CHECK(diagnostic.niters == 138);
    const auto solution = pfitsio::read2d(expected_solution_path);
    const auto residual = pfitsio::read2d(expected_residual_path);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-6));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-6));
  }
}
