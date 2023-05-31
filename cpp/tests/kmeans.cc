#include "catch.hpp"

#include "purify/config.h"
#include "purify/types.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/utilities.h"

#include <sopt/mpi/communicator.h>

using namespace purify;

TEST_CASE("k-means") {
  auto const world = sopt::mpi::Communicator::World();
  t_int const number_of_groups = world.size();
  t_int const N = 1e5;

  CAPTURE(world.rank());
  CAPTURE(world.size());
  utilities::vis_params uv_data;

  uv_data.u = world.broadcast<Vector<t_real>>(Vector<t_real>::Random(N));
  uv_data.v = world.broadcast<Vector<t_real>>(Vector<t_real>::Random(N));
  uv_data.w = world.broadcast<Vector<t_real>>(Vector<t_real>::Random(N));
  uv_data.vis = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(N));
  uv_data.weights = world.broadcast<Vector<t_complex>>(Vector<t_complex>::Random(N));

  const auto kmeans_result_serial = distribute::kmeans_algo(uv_data.w, number_of_groups, 100);
  const std::vector<t_int> serial_index = std::get<0>(kmeans_result_serial);
  const std::vector<t_real> serial_means = std::get<1>(kmeans_result_serial);
  REQUIRE(serial_means.size() == number_of_groups);

  t_int const start = world.rank() * (N - (N % world.size())) / world.size();
  t_int const length = (N - (N % world.size())) / world.size() +
                       ((world.rank() == (world.size() - 1)) ? (N % world.size()) : 0);
  CAPTURE(start);
  CAPTURE(length);
  CAPTURE(N % world.size());
  const Vector<t_real> w_segment = uv_data.w.segment(start, length);
  CAPTURE(w_segment.size());
  const auto kmeans_result_mpi = distribute::kmeans_algo(w_segment, number_of_groups, 100, world);
  const std::vector<t_int> mpi_index = std::get<0>(kmeans_result_mpi);
  const std::vector<t_real> mpi_means = std::get<1>(kmeans_result_mpi);
  REQUIRE(mpi_means.size() == number_of_groups);
  REQUIRE(w_segment.size() == length);
  // Check that serial and mpi kmeans give the same results
  for (t_int j = 0; j < length; j++) {
    CAPTURE(j);
    CAPTURE(start + j);
    REQUIRE(serial_index.at(start + j) == mpi_index.at(j));
  }
  for (t_int g = 0; g < number_of_groups; g++)
    REQUIRE(mpi_means.at(g) == Approx(serial_means.at(g)));
  // Check that mean values are the same
  auto uv_dist_all =
      utilities::regroup_and_all_to_all(uv_data.segment(start, length), mpi_index, world);
  auto uv_dist_scatter = utilities::regroup_and_scatter(uv_data, serial_index, world);
  // check w values are distributed right between mpi and non mpi versions
#define TEST_MACRO(param)                                \
  {                                                      \
    Vector<t_real> a = uv_dist_all.param.cwiseAbs();     \
    Vector<t_real> b = uv_dist_scatter.param.cwiseAbs(); \
    std::sort(a.data(), a.data() + a.size());            \
    std::sort(b.data(), b.data() + b.size());            \
    CAPTURE(a.head(5));                                  \
    CAPTURE(b.head(5));                                  \
    REQUIRE(a.isApprox(b));                              \
  }
  TEST_MACRO(w)
  TEST_MACRO(v)
  TEST_MACRO(u)
  TEST_MACRO(vis)
  TEST_MACRO(weights)
#undef TEST_MACRO
}
TEST_CASE("distribute w") {
  purify::logging::set_level("debug");
  const t_uint M = 1000;
  const t_int min_support = 4;
  const t_int max_support = 100;
  const t_real du = 1;
  auto const comm = sopt::mpi::Communicator::World();

  const auto params = utilities::random_sample_density(M, 0, constant::pi / 3, 100);
  const auto kmeans = distribute::kmeans_algo(
      params.w, comm.size(), 100, comm, [](t_real x) { return x * x; }, 1e-5);
  const std::vector<t_int> image_index = std::get<0>(kmeans);
  const std::vector<t_real> w_stacks = std::get<1>(kmeans);
  const std::vector<t_int> groups =
      distribute::w_support(params.w, image_index, w_stacks, du, min_support, max_support, 0, comm);
  auto sorted = utilities::regroup_and_all_to_all(params, image_index, groups, comm);
  const auto data = utilities::w_stacking_with_all_to_all(
      params, du, min_support, max_support, comm, 100, 0, [](t_real x) { return x * x; }, 1e-5);
  CHECK(std::get<0>(sorted).u.isApprox(std::get<0>(data).u));
  CHECK(std::get<0>(sorted).v.isApprox(std::get<0>(data).v));
  CHECK(std::get<0>(sorted).w.isApprox(std::get<0>(data).w));
  CHECK(std::get<1>(sorted) == std::get<1>(data));
  CHECK(w_stacks == std::get<2>(data));
}
