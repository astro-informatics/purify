#include <numeric>
#include <random>
#include <utility>
#include <catch2/catch_all.hpp>

#include "purify/types.h"
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/logging.h"
#include "purify/mpi_utilities.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include <sopt/logging.h>
#include <sopt/mpi/communicator.h>
#include <sopt/mpi/utilities.h>
#include <sopt/power_method.h>
#include <sopt/wavelets.h>

#ifdef PURIFY_H5
#include "purify/h5reader.h"
#endif

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
    auto const order = distribute::distribute_measurements(result, comm, distribute::plan::none);
    return utilities::regroup_and_scatter(result, order, comm);
  }
  auto result = utilities::scatter_visibilities(comm);
  return result;
}

TEST_CASE("Serial vs. Serial with MPI PADMM") {
  auto const world = sopt::mpi::Communicator::World();

  const std::string &test_dir = "expected/padmm/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");

  auto uv_data = dirty_visibilities({input_data_path}, world);
  uv_data.units = utilities::vis_units::radians;
  if (world.is_root()) {
    CAPTURE(uv_data.vis.head(5));
  }
  REQUIRE(world.all_sum_all(uv_data.size()) == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey, imsizex, 1,
      1, 2, kernels::kernel_from_string.at("kb"), 4, 4);
  Vector<t_complex> const init = Vector<t_complex>::Ones(imsizex * imsizey).eval();
  auto const power_method_stuff =
      sopt::algorithm::power_method<Vector<t_complex>>(*measurements_transform, 1000, 1e-5, init);
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex);
  t_real const sigma =
      world.broadcast(0.016820222945913496) * std::sqrt(2);  // see test_parameters file
  SECTION("global") {
    auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data, sigma,
        imsizey, imsizex, sara.size(), 300, true, true, false, 1e-2, 1e-3, 50, 1, op_norm);

    auto const diagnostic = (*padmm)();
    CHECK(diagnostic.niters == 10);

    const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

    const auto solution = pfitsio::read2d(expected_solution_path);
    const auto residual = pfitsio::read2d(expected_residual_path);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-4));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
  }
  SECTION("local") {
    auto const padmm = factory::padmm_factory<sopt::algorithm::ImagingProximalADMM<t_complex>>(
        factory::algo_distribution::mpi_distributed, measurements_transform, wavelets, uv_data,
        sigma, imsizey, imsizex, sara.size(), 500, true, true, false, 1e-2, 1e-3, 50, 1, op_norm);

    auto const diagnostic = (*padmm)();
    t_real const epsilon = utilities::calculate_l2_radius(world.all_sum_all(uv_data.vis.size()),
                                                          world.broadcast(sigma));
    CHECK(sopt::mpi::l2_norm(diagnostic.residual, padmm->l2ball_proximal_weights(), world) <
          epsilon);
    // the algorithm depends on nodes, so other than a basic bounds check,
    // it is hard to know exact precision (might depend on probability theory...)
    if (world.size() > 2 or world.size() == 0) return;
    // testing the case where there are two nodes exactly.
    const std::string &expected_solution_path = (world.size() == 2)
                                                    ? data_filename(test_dir + "mpi_solution.fits")
                                                    : data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path = (world.size() == 2)
                                                    ? data_filename(test_dir + "mpi_residual.fits")
                                                    : data_filename(test_dir + "residual.fits");
    if (world.size() == 1) CHECK(diagnostic.niters == 10);
    if (world.size() == 2) CHECK(diagnostic.niters == 11);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    // if (world.is_root()) pfitsio::write2d(image.real(), expected_solution_path);
    const auto solution = pfitsio::read2d(expected_solution_path);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-4));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    // if (world.is_root()) pfitsio::write2d(residual_image.real(), expected_residual_path);
    const auto residual = pfitsio::read2d(expected_residual_path);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
  }
}

TEST_CASE("Serial vs. Serial with MPI Primal Dual", "[!shouldfail]") {
  auto const world = sopt::mpi::Communicator::World();

  const std::string &test_dir = "expected/primal_dual/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");

  auto uv_data = dirty_visibilities({input_data_path}, world);
  uv_data.units = utilities::vis_units::radians;
  if (world.is_root()) {
    CAPTURE(uv_data.vis.head(5));
  }
  REQUIRE(world.all_sum_all(uv_data.size()) == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey, imsizex, 1,
      1, 2, kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
      *measurements_transform, 1000, 1e-5,
      world.broadcast(Vector<t_complex>::Ones(imsizex * imsizey).eval()));
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex);
  t_real const sigma =
      world.broadcast(0.016820222945913496) * std::sqrt(2);  // see test_parameters file
  SECTION("global") {
    auto const primaldual =
        factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
            factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data,
            sigma, imsizey, imsizex, sara.size(), 500, true, true, 1e-2, 1, op_norm);

    auto const diagnostic = (*primaldual)();
    CHECK(diagnostic.niters == 16);

    const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

    const auto solution = pfitsio::read2d(expected_solution_path);
    const auto residual = pfitsio::read2d(expected_residual_path);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-4));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
  }
  SECTION("local") {
    auto const primaldual =
        factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
            factory::algo_distribution::mpi_distributed, measurements_transform, wavelets, uv_data,
            sigma, imsizey, imsizex, sara.size(), 500, true, true, 1e-2, 1, op_norm);

    auto const diagnostic = (*primaldual)();
    t_real const epsilon = utilities::calculate_l2_radius(world.all_sum_all(uv_data.vis.size()),
                                                          world.broadcast(sigma));
    CHECK(sopt::mpi::l2_norm(diagnostic.residual, primaldual->l2ball_proximal_weights(), world) <
          epsilon);
    // the algorithm depends on nodes, so other than a basic bounds check,
    // it is hard to know exact precision (might depend on probability theory...)
    if (world.size() > 2 or world.size() == 0) return;
    // testing the case where there are two nodes exactly.
    const std::string &expected_solution_path = (world.size() == 2)
                                                    ? data_filename(test_dir + "mpi_solution.fits")
                                                    : data_filename(test_dir + "solution.fits");
    const std::string &expected_residual_path = (world.size() == 2)
                                                    ? data_filename(test_dir + "mpi_residual.fits")
                                                    : data_filename(test_dir + "residual.fits");
    if (world.size() == 1) CHECK(diagnostic.niters == 16);
    if (world.size() == 2) CHECK(diagnostic.niters == 18);
    const auto solution = pfitsio::read2d(expected_solution_path);
    const auto residual = pfitsio::read2d(expected_residual_path);

    const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
    // if (world.is_root()) pfitsio::write2d(image.real(), expected_solution_path);
    CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
    CHECK(image.isApprox(solution, 1e-4));

    const Vector<t_complex> residuals = measurements_transform->adjoint() *
                                        (uv_data.vis - ((*measurements_transform) * diagnostic.x));
    const Image<t_complex> residual_image =
        Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
    // if (world.is_root()) pfitsio::write2d(residual_image.real(), expected_residual_path);
    CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
    CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
    CHECK(residual_image.real().isApprox(residual.real(), 1e-4));
  }
  SECTION("random update") {
    auto const measurements_transform_serial =
        factory::measurement_operator_factory<Vector<t_complex>>(
            factory::distributed_measurement_operator::serial, uv_data, imsizey, imsizex, 1, 1, 2,
            kernels::kernel_from_string.at("kb"), 4, 4);
    auto const power_method_stuff = sopt::algorithm::all_sum_all_power_method<Vector<t_complex>>(
        world, *measurements_transform, 1000, 1e-5,
        world.broadcast(Vector<t_complex>::Ones(imsizex * imsizey).eval()));
    const t_real op_norm = std::get<0>(power_method_stuff);
    auto sara_dist = sopt::wavelets::distribute_sara(sara, world);
    auto const wavelets_serial = factory::wavelet_operator_factory<Vector<t_complex>>(
        factory::distributed_wavelet_operator::serial, sara_dist, imsizey, imsizex);

    auto const primaldual =
        factory::primaldual_factory<sopt::algorithm::ImagingPrimalDual<t_complex>>(
            factory::algo_distribution::mpi_random_updates, measurements_transform_serial,
            wavelets_serial, uv_data, sigma, imsizey, imsizex, sara_dist.size(), 500, true, true,
            1e-2, 1, op_norm);

    auto const diagnostic = (*primaldual)();
    t_real const epsilon = utilities::calculate_l2_radius(world.all_sum_all(uv_data.vis.size()),
                                                          world.broadcast(sigma));
    CHECK(sopt::mpi::l2_norm(diagnostic.residual, primaldual->l2ball_proximal_weights(), world) <
          epsilon);
    if (world.size() > 1) return;
    // the algorithm depends on nodes, so other than a basic bounds check,
    // it is hard to know exact precision (might depend on probability theory...)
    if (world.size() == 0)
      return;
    else if (world.size() == 2 or world.size() == 1) {
      // testing the case where there are two nodes exactly.
      const std::string &expected_solution_path =
          (world.size() == 2) ? data_filename(test_dir + "mpi_random_solution.fits")
                              : data_filename(test_dir + "solution.fits");
      const std::string &expected_residual_path =
          (world.size() == 2) ? data_filename(test_dir + "mpi_random_residual.fits")
                              : data_filename(test_dir + "residual.fits");
      if (world.size() == 1) CHECK(diagnostic.niters == 16);
      if (world.size() == 2) CHECK(diagnostic.niters < 100);

      const auto solution = pfitsio::read2d(expected_solution_path);
      const auto residual = pfitsio::read2d(expected_residual_path);

      const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
      // if (world.is_root()) pfitsio::write2d(image.real(), expected_solution_path);
      CAPTURE(Vector<t_complex>::Map(solution.data(), solution.size()).real().head(10));
      CAPTURE(Vector<t_complex>::Map(image.data(), image.size()).real().head(10));
      CAPTURE(
          Vector<t_complex>::Map((image / solution).eval().data(), image.size()).real().head(10));
      CHECK(image.isApprox(solution, 1e-3));

      const Vector<t_complex> residuals =
          measurements_transform->adjoint() *
          (uv_data.vis - ((*measurements_transform) * diagnostic.x));
      const Image<t_complex> residual_image =
          Image<t_complex>::Map(residuals.data(), imsizey, imsizex);
      // if (world.is_root()) pfitsio::write2d(residual_image.real(), expected_residual_path);
      CAPTURE(Vector<t_complex>::Map(residual.data(), residual.size()).real().head(10));
      CAPTURE(Vector<t_complex>::Map(residuals.data(), residuals.size()).real().head(10));
      CHECK(residual_image.real().isApprox(residual.real(), 1e-3));
    } else
      return;
  }
}

TEST_CASE("Serial vs. Serial with MPI Forward Backward") {
  auto const world = sopt::mpi::Communicator::World();

  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.vis");
  const std::string &result_path = data_filename(test_dir + "mpi_fb_result.fits");

  auto uv_data = dirty_visibilities({input_data_path}, world);
  uv_data.units = utilities::vis_units::radians;
  if (world.is_root()) {
    CAPTURE(uv_data.vis.head(5));
  }
  REQUIRE(world.all_sum_all(uv_data.size()) == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey, imsizex, 1,
      1, 2, kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
      *measurements_transform, 1000, 1e-5,
      world.broadcast(Vector<t_complex>::Ones(imsizex * imsizey).eval()));
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex);
  t_real const sigma =
      world.broadcast(0.016820222945913496) * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;
  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data, sigma,
      beta, gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, op_norm);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  if (world.is_root())
  {
    pfitsio::write2d(image.real(), result_path);
  //pfitsio::write2d(residual_image.real(), expected_residual_path);
  }

  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  double average_intensity = diagnostic.x.real().sum() / diagnostic.x.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (Vector<t_complex>::Map(solution.data(), solution.size()) - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}

TEST_CASE("MPI_fb_factory_hdf5") {
  auto const world = sopt::mpi::Communicator::World();
  const size_t N = 13107;
  
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.h5");
  const std::string &result_path = data_filename(test_dir + "mpi_fb_result.fits");
  H5::H5Handler h5file(input_data_path, world);

  auto uv_data = H5::stochread_visibility(h5file, 6000, false);
  uv_data.units = utilities::vis_units::radians;
  if (world.is_root()) {
    CAPTURE(uv_data.vis.head(5));
  }
  //REQUIRE(world.all_sum_all(uv_data.size()) == 13107);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;

  auto const measurements_transform = factory::measurement_operator_factory<Vector<t_complex>>(
      factory::distributed_measurement_operator::mpi_distribute_image, uv_data, imsizey, imsizex, 1,
      1, 2, kernels::kernel_from_string.at("kb"), 4, 4);
  auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
      *measurements_transform, 1000, 1e-5,
      world.broadcast(Vector<t_complex>::Ones(imsizex * imsizey).eval()));
  const t_real op_norm = std::get<0>(power_method_stuff);
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::mpi_sara, sara, imsizey, imsizex);
  t_real const sigma =
      world.broadcast(0.016820222945913496) * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;
  auto const fb = factory::fb_factory<sopt::algorithm::ImagingForwardBackward<t_complex>>(
      factory::algo_distribution::mpi_serial, measurements_transform, wavelets, uv_data, sigma,
      beta, gamma, imsizey, imsizex, sara.size(), 1000, true, true, false, 1e-2, 1e-3, 50, op_norm);

  auto const diagnostic = (*fb)();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  if (world.is_root())
  {
    pfitsio::write2d(image.real(), result_path);
  //pfitsio::write2d(residual_image.real(), expected_residual_path);
  }

  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &expected_residual_path = data_filename(test_dir + "residual.fits");

  const auto solution = pfitsio::read2d(expected_solution_path);
  const auto residual = pfitsio::read2d(expected_residual_path);

  double average_intensity = diagnostic.x.real().sum() / diagnostic.x.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (Vector<t_complex>::Map(solution.data(), solution.size()) - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}

#ifdef PURIFY_H5
TEST_CASE("fb_factory_stochastic") {
  const std::string &test_dir = "expected/fb/";
  const std::string &input_data_path = data_filename(test_dir + "input_data.h5");
  const std::string &expected_solution_path = data_filename(test_dir + "solution.fits");
  const std::string &result_path = data_filename(test_dir + "fb_stochastic_result_mpi.fits");
  
  // HDF5
  auto const comm = sopt::mpi::Communicator::World();
  const size_t N = 2000;
  H5::H5Handler h5file(input_data_path, comm);  // length 13107
  using t_complexVec = Vector<t_complex>;

  // This functor would be defined in Purify
  std::function<std::shared_ptr<sopt::IterationState<Vector<t_complex>>>()> random_updater = [&f = h5file, &N]() {
    utilities::vis_params uv_data = H5::stochread_visibility(f, N, false);  // no w-term in this data-set
    uv_data.units = utilities::vis_units::radians;
    auto phi = factory::measurement_operator_factory<t_complexVec>(
        factory::distributed_measurement_operator::mpi_distribute_image, uv_data, 128, 128, 1, 1, 2,
        kernels::kernel_from_string.at("kb"), 4, 4);

    return std::make_shared<sopt::IterationState<Vector<t_complex>>>(uv_data.vis, phi);
  };

  auto IS = random_updater();
  auto Phi = IS->Phi();
  auto const power_method_stuff = sopt::algorithm::power_method<Vector<t_complex>>(
      Phi, 1000, 1e-5,
      comm.broadcast(Vector<t_complex>::Ones(128 * 128).eval()));
  const t_real op_norm = std::get<0>(power_method_stuff);

  const auto solution = pfitsio::read2d(expected_solution_path);

  t_uint const imsizey = 128;
  t_uint const imsizex = 128;
  
  //wavelets
  std::vector<std::tuple<std::string, t_uint>> const sara{
      std::make_tuple("Dirac", 3u), std::make_tuple("DB1", 3u), std::make_tuple("DB2", 3u),
      std::make_tuple("DB3", 3u),   std::make_tuple("DB4", 3u), std::make_tuple("DB5", 3u),
      std::make_tuple("DB6", 3u),   std::make_tuple("DB7", 3u), std::make_tuple("DB8", 3u)};
  auto const wavelets = factory::wavelet_operator_factory<Vector<t_complex>>(
      factory::distributed_wavelet_operator::serial, sara, imsizey, imsizex);
  
  //algorithm
  t_real const sigma = 0.016820222945913496 * std::sqrt(2);  // see test_parameters file
  t_real const beta = sigma * sigma;
  t_real const gamma = 0.0001;

  sopt::algorithm::ImagingForwardBackward<t_complex> fb(random_updater);
  fb.itermax(1000).step_size(beta*sqrt(2)).sigma(sigma*sqrt(2)).regulariser_strength(gamma).relative_variation(1e-3).residual_tolerance(0).tight_frame(true).sq_op_norm(op_norm*op_norm).obj_comm(comm);

  auto gp = std::make_shared<sopt::algorithm::L1GProximal<t_complex>>(false);
  gp->l1_proximal_tolerance(1e-4).l1_proximal_nu(1).l1_proximal_itermax(50).l1_proximal_positivity_constraint(true).l1_proximal_real_constraint(true).Psi(*wavelets);
  fb.g_function(gp);

  auto const diagnostic = fb();
  const Image<t_complex> image = Image<t_complex>::Map(diagnostic.x.data(), imsizey, imsizex);
  SOPT_HIGH_LOG("God help me.");
  if (comm.is_root())
  {
    SOPT_HIGH_LOG("Root write file");
    pfitsio::write2d(image.real(), result_path);
  //pfitsio::write2d(residual_image.real(), expected_residual_path);
  }
  else
  {
    SOPT_HIGH_LOG("Worker has nowt to do.");
  }

  auto soln_flat = Vector<t_complex>::Map(solution.data(), solution.size());
  double average_intensity = soln_flat.real().sum() / soln_flat.size();
  SOPT_HIGH_LOG("Average intensity = {}", average_intensity);
  double mse = (soln_flat - diagnostic.x)
                   .real()
                   .squaredNorm() /
               solution.size();
  SOPT_HIGH_LOG("MSE = {}", mse);
  CHECK(mse <= average_intensity * 1e-3);
}
#endif