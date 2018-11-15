#include <set>
#include <purify/AllToAllSparseVector.h>
#include <purify/DistributeSparseVector.h>
#include "catch.hpp"
#include <sopt/mpi/communicator.h>
using namespace purify;

TEST_CASE("Distribe Sparse Vector") {
  auto const world = sopt::mpi::Communicator::World();
  Vector<t_int> const grid = world.broadcast<Vector<t_int>>(
      Vector<t_int>::Random(std::max(world.size() * 2, world.size() + 2)));
  std::vector<t_int> const indices = {static_cast<t_int>(world.rank()),
                                      static_cast<t_int>(world.size() + 1)};
  DistributeSparseVector distributor(indices, grid.size(), world);

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
    if (world.is_root()) {
      distributor.gather(local, output);
      CHECK(output.size() == grid.size());
      for (decltype(world.size()) i(0); i < world.size(); ++i) CHECK(output(i) == i);
      CHECK(output(world.size()) == 0);
      CHECK(output(world.size() + 1) ==
            world.size() * world.size() + (world.size() * (world.size() - 1)) / 2);
      CHECK(grid.size() - 2 >= world.size());
      CHECK((output.tail(grid.size() - world.size() - 2).array() == 0).all());
    } else {
      distributor.gather(local);
    }
  }
}
TEST_CASE("All to one Sparse Vector") {
  auto const world = sopt::mpi::Communicator::World();
  Vector<t_int> const grid = world.broadcast<Vector<t_int>>(
      Vector<t_int>::Random(std::max(world.size() * 2, world.size() + 2)));
  std::vector<t_int> const indices = {static_cast<t_int>(world.rank()),
                                      static_cast<t_int>(world.size() + 1)};
  AllToAllSparseVector distributor(indices, grid.size(), grid.size() * world.rank(), world);

  SECTION("Scatter") {
    Vector<t_int> output;
    distributor.recv_grid(grid, output);
    CAPTURE(world.rank());
    REQUIRE(output.size() == 2);
    CHECK(output(0) == grid(world.rank()));
    CHECK(output(1) == grid(world.size() + 1));
  }

  SECTION("Gather") {
    Vector<t_int> local(2);
    local << world.rank(), world.size() + world.rank();
    Vector<t_int> output;
    distributor.send_grid(local, output);
    CHECK(output.size() == grid.size());
    if (world.is_root()) {
      for (decltype(world.size()) i(0); i < world.size(); ++i) CHECK(output(i) == i);
      CHECK(output(world.size()) == 0);
      CHECK(output(world.size() + 1) ==
            world.size() * world.size() + (world.size() * (world.size() - 1)) / 2);
      CHECK(grid.size() - 2 >= world.size());
      CHECK((output.tail(grid.size() - world.size() - 2).array() == 0).all());
    } else {
      for (t_int i = 0; i < output.size(); i++) CHECK(output(i) == 0);
    }
  }
}

TEST_CASE("All to All Sparse Vector") {
  auto const world = sopt::mpi::Communicator::World();
  CAPTURE(world.rank());
  Vector<t_int> const grid = world.broadcast<Vector<t_int>>(
      Vector<t_int>::Random(std::max(world.size() * 2, world.size() + 2)));
  SECTION("Check throw when index is not ordered by node") {
    std::vector<t_int> const indices = {
        static_cast<t_int>(grid.size() * (world.rank() + 1) + world.size() + 1),
        static_cast<t_int>(world.rank())};
    CHECK_THROWS(AllToAllSparseVector(indices, grid.size(), grid.size() * world.rank(), world));
  }
  std::vector<t_int> const indices = {
      static_cast<t_int>(world.rank()),
      static_cast<t_int>(grid.size() * world.rank() + world.size() + 1)};
  AllToAllSparseVector distributor(indices, grid.size(), grid.size() * world.rank(), world);

  SECTION("Scatter") {
    Vector<t_int> output;
    distributor.recv_grid(grid, output);
    REQUIRE(output.size() == 2);
    CHECK(output(0) == grid(world.rank()));
    CHECK(output(1) == grid(world.size() + 1));
  }

  SECTION("Gather") {
    Vector<t_int> local = Vector<t_int>::Ones(2);
    Vector<t_int> output;
    distributor.send_grid(local, output);
    CHECK(output.size() == grid.size());
    CAPTURE(world.rank());
    CAPTURE(output);
    if (world.is_root()) {
      for (decltype(world.size()) i(0); i < world.size(); ++i) CHECK(output(i) == 1);
      CHECK(output(world.size()) == 0);
      CHECK(output(world.size() + 1) == 1);
      CHECK(grid.size() - 2 >= world.size());
      CHECK((output.tail(grid.size() - world.size() - 2).array() == 0).all());
    } else {
      for (t_int i = 0; i < output.size(); i++) {
        if (i != world.size() + 1)
          CHECK(output(i) == 0);
        else
          CHECK(output(i) == 1);
      }
    }
  }
}
