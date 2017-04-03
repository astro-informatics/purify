
#include "purify/config.h"
#include "catch.hpp"
#include "purify/types.h"
#include "purify/IndexMapping.h"
#include <numeric>

using namespace purify;

TEST_CASE("") {

  std::set<t_int> indices{1, 3, 5, 2};
  auto mapper = IndexMapping(indices, 10);
  Vector<t_int> input(10);
  Vector<t_int> output;

  std::iota(input.data(), input.data() + input.size(), 0);


  mapper(input, output);
  SECTION("Direct application") {
    CHECK(output.size() == 4);
    decltype(output)::Index i(0);
    for(auto const &index: indices)
      CHECK(output(i++) == index);
  }

  Vector<t_int> adjoint;
  mapper.adjoint(output, adjoint);
  SECTION("Adjoint application") {
    CHECK(adjoint.size() == 10);
    for(auto const &index: indices) {
      CHECK(adjoint(index) == index);
      adjoint(index) = 0;
    }
    CHECK(adjoint == adjoint.Zero(adjoint.size()));
  }
}
