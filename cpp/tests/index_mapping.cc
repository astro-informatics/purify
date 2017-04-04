#include <iostream>
#include "purify/config.h"
#include <numeric>
#include "catch.hpp"
#include "purify/IndexMapping.h"
#include "purify/types.h"

using namespace purify;

TEST_CASE("Index mapping operator") {
  std::set<t_int> indices{1, 3, 5, 2};
  auto mapper = IndexMapping(indices, 10);
  Vector<t_int> input(10);
  Vector<t_int> output;

  std::iota(input.data(), input.data() + input.size(), 0);

  mapper(input, output);
  SECTION("Direct application") {
    CHECK(output.size() == 4);
    decltype(output)::Index i(0);
    for(auto const &index : indices)
      CHECK(output(i++) == index);
  }

  Vector<t_int> adjoint;
  mapper.adjoint(output, adjoint);
  SECTION("Adjoint application") {
    CHECK(adjoint.size() == 10);
    for(auto const &index : indices) {
      CHECK(adjoint(index) == index);
      adjoint(index) = 0;
    }
    CHECK(adjoint == adjoint.Zero(adjoint.size()));
  }
}

TEST_CASE("Non empty outer vectors") {
  Sparse<t_int> matrix(4, 4);
  std::vector<Eigen::Triplet<t_int>> const triplets = {{0, 0, 1}, {0, 3, 1}, {2, 3, 1}};
  matrix.setFromTriplets(triplets.begin(), triplets.end());

  auto const indices = non_empty_outers(matrix);
  CHECK(indices.size() == 2);
  CHECK(indices.count(0) == 1);
  CHECK(indices.count(3) == 1);
}
