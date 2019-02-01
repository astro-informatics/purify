#include "purify/mpi_utilities.h"
#include "catch.hpp"

using namespace purify;

TEST_CASE("re-group visibilities in order of processes") {
  auto const N = 5;
  utilities::vis_params params;

  params.u = Vector<t_real>::Random(N);
  params.v = Vector<t_real>::Random(N);
  params.w = Vector<t_real>::Random(N);
  params.vis = Vector<t_complex>::Random(N);
  params.weights = Vector<t_complex>::Random(N);

  std::vector<t_int> order(N);

  SECTION("Single item out of order") {
    order[0] = 1;
    std::fill(order.begin() + 1, order.end(), 0);

    auto actual = params;
    utilities::regroup(actual, order, 2);
    CHECK(params.u(0) == Approx(actual.u(N - 1)));
    CHECK(params.u(N - 1) == Approx(actual.u(0)));
    CHECK(params.u.segment(1, N - 2).isApprox(actual.u.segment(1, N - 2)));
  }

  SECTION("Two items out of order") {
    order[0] = 0;
    order[1] = 1;
    order[2] = 1;
    std::fill(order.begin() + 3, order.end(), 0);

    auto actual = params;
    utilities::regroup(actual, order, 2);
    CHECK(params.u(0) == Approx(actual.u(0)));
    CHECK(params.u(1) == Approx(actual.u(N - 2)));
    CHECK(params.u(2) == Approx(actual.u(N - 1)));
    CHECK(params.u(3) == Approx(actual.u(1)));
    CHECK(params.u(4) == Approx(actual.u(2)));
  }

  SECTION("Three procs") {
    order = {1, 0, 2, 2, 0};
    auto actual = params;
    utilities::regroup(actual, order, 3);
    CHECK(actual.u(0) == Approx(params.u(N - 1)));
    CHECK(actual.u(1) == Approx(params.u(1)));
    CHECK(actual.u(2) == Approx(params.u(0)));
    CHECK(actual.u(3) == Approx(params.u(3)));
    CHECK(actual.u(4) == Approx(params.u(2)));
  }
}
