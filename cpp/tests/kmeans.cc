#include "catch.hpp"

#include "purify/config.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include "purify/types.h"
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

  const std::vector<t_int> serial_index = distribute::w_patition(uv_data.w, number_of_groups, 100);

  t_int const start = world.rank() * (N - (N % world.size())) / world.size();
  t_int const length = (N - (N % world.size())) / world.size()
                       + ((world.rank() == (world.size() - 1)) ? (N % world.size()) : 0);
  CAPTURE(start);
  CAPTURE(length);
  CAPTURE(N % world.size());
  const Vector<t_real> w_segment = uv_data.w.segment(start, length);
  CAPTURE(w_segment.size());
  const std::vector<t_int> mpi_index
      = distribute::w_patition(w_segment, number_of_groups, 100, world);
  REQUIRE(w_segment.size() == length);
  // Check that serial and mpi kmeans give the same results
  for(t_int j = 0; j < length; j++) {
    CAPTURE(j);
    CAPTURE(start + j);
    REQUIRE(serial_index.at(start + j) == mpi_index.at(j));
  }
  auto uv_dist_all
      = utilities::regroup_and_all_to_all(uv_data.segment(start, length), mpi_index, world);
  auto uv_dist_scatter = utilities::regroup_and_scatter(uv_data, serial_index, world);
  // check w values are distributed right between mpi and non mpi versions
#define TEST_MACRO(param)                                                                          \
  {                                                                                                \
    Vector<t_real> a = uv_dist_all.param.cwiseAbs();                                               \
    Vector<t_real> b = uv_dist_scatter.param.cwiseAbs();                                           \
    std::sort(a.data(), a.data() + a.size());                                                      \
    std::sort(b.data(), b.data() + b.size());                                                      \
    CAPTURE(a.head(5));                                                                            \
    CAPTURE(b.head(5));                                                                            \
    REQUIRE(a.isApprox(b));                                                                        \
  }
  TEST_MACRO(w)
  TEST_MACRO(v)
  TEST_MACRO(u)
  TEST_MACRO(vis)
  TEST_MACRO(weights)
#undef TEST_MACRO
}
