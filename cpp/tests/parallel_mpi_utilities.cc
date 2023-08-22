#include "catch2/catch_all.hpp"
#include "purify/mpi_utilities.h"

using namespace purify;
using Catch::Approx;

TEST_CASE("Distribution of visibility data") {
  auto const world = sopt::mpi::Communicator::World();
  auto const N = 5;
  utilities::vis_params params;

  params.u = Vector<t_real>::Random(N);
  params.v = Vector<t_real>::Random(N);
  params.w = Vector<t_real>::Random(N);
  params.vis = Vector<t_complex>::Random(N);
  params.weights = Vector<t_complex>::Random(N);

  std::vector<t_int> order(N);

  if (world.size() == 1) return;
  order[0] = 1;
  std::fill(order.begin() + 1, order.end(), 0);

  auto actual = utilities::regroup_and_scatter(params, order, world);
  if (world.rank() == 0) {
    CHECK(actual.u.size() == 4);
    CHECK(actual.u(0) == Approx(params.u(N - 1)));
    CHECK(actual.u.tail(N - 2).isApprox(params.u.segment(1, N - 2)));
  } else if (world.rank() == 1) {
    CHECK(actual.u.size() == 1);
    CHECK(actual.u(0) == Approx(params.u(0)));
  } else
    CHECK(actual.u.size() == 0);
}
