
#include <iomanip>
#include <sopt/mpi/communicator.h>
#include "catch.hpp"
#include <purify/DistributeSparseVector.h>
#include <set>
using namespace purify;


TEST_CASE("Distribe Sparse Vector") {
  auto const world = sopt::mpi::Communicator::World();
  Vector<t_int> const grid = world.broadcast<Vector<t_int>>(Vector<t_int>::Random(world.size() * 2));
  std::vector<t_int> const indices = {static_cast<t_int>(world.rank()), static_cast<t_int>(world.size() + 1)};
  DistributeSparseVector distributor(indices, 2 * world.size(), world);

  SECTION("Scatter") {
    Vector<t_int> output;
    distributor.scatter(grid, output);
    REQUIRE(output.size() == 2);
    CHECK(output(0) == grid(world.rank()));
    CHECK(output(1) == grid(world.size() + 1));
  }

  SECTION("Gather") {
    Vector<t_int> local(2);
    local << world.rank(), world.size() + world.rank();
    Vector<t_int> output;
    if(world.is_root()) {
      distributor.gather(local, output);
      CHECK(output.size() == world.size() * 2);
      for(t_int i(0); i < world.size(); ++i)
        CHECK(output(i) == i);
      CHECK(output(world.size()) == 0);
      CHECK(output(world.size() + 1) == world.size() * world.size() + (world.size() * (world.size() - 1)) / 2);
      CHECK(output.tail(world.size() - 2) == output.Zero(world.size() - 2));
    } else {
      distributor.gather(local);
    }
  }
}
