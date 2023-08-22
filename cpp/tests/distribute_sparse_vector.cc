#include <random>
#include <set>
#include <purify/AllToAllSparseVector.h>
#include <purify/DistributeSparseVector.h>
#include "catch2/catch_all.hpp"
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
  AllToAllSparseVector<t_int> distributor(indices, grid.size(), grid.size() * world.rank(), world);

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
    CHECK_THROWS(
        AllToAllSparseVector<t_int>(indices, grid.size(), grid.size() * world.rank(), world));
  }
  std::vector<t_int> const indices = {
      static_cast<t_int>(world.rank()),
      static_cast<t_int>(grid.size() * world.rank() + world.size() + 1)};
  AllToAllSparseVector<t_int> distributor(indices, grid.size(), grid.size() * world.rank(), world);

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

TEST_CASE("recv_sizes") {
  for (t_int nodes : {1, 2, 5, 10, 20, 50, 100, 1000}) {
    for (t_int imsize : {128, 1024, 2048, 4096, 8192, 16384, 32768}) {
      const std::int32_t N = imsize * imsize;
      CAPTURE(imsize);
      CAPTURE(N);
      // First create an instance of an engine.
      std::random_device rnd_device;
      // Specify the engine and distribution.
      std::mt19937_64 mersenne_engine(rnd_device());  // Generates random integers
      std::uniform_int_distribution<t_int> dist(0, nodes * N);
      auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };
      std::vector<t_int> local_indices(10);
      std::generate(local_indices.begin(), local_indices.end(), gen);
      std::sort(local_indices.begin(), local_indices.end(),
                [](t_int a, t_int b) { return (a < b); });
      CAPTURE(local_indices);
      CAPTURE(nodes);
      if (static_cast<std::int64_t>(N) * static_cast<std::int64_t>(nodes) >
          std::numeric_limits<t_int>::max())
        CHECK_THROWS(all_to_all_recv_sizes<t_int>(local_indices, nodes, N));
      else {
        std::vector<t_int> recv = all_to_all_recv_sizes<t_int>(local_indices, nodes, N);
        for (const auto& a : recv) CHECK(a >= 0);

        CHECK(local_indices.size() == std::accumulate(recv.begin(), recv.end(), 0));
      }
    }
  }
}
TEST_CASE("recv_sizes 64 bit") {
  for (std::int64_t nodes : {1, 2, 5, 10, 20, 50, 100, 1000}) {
    for (std::int64_t imsize : {128, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072}) {
      const std::int64_t N = imsize * imsize;
      CAPTURE(imsize);
      // First create an instance of an engine.
      std::random_device rnd_device;
      // Specify the engine and distribution.
      std::mt19937_64 mersenne_engine(rnd_device());  // Generates random integers
      std::uniform_int_distribution<std::int64_t> dist(
          0, static_cast<std::int64_t>(nodes) * static_cast<std::int64_t>(N));
      auto gen = [&dist, &mersenne_engine]() -> std::int64_t { return dist(mersenne_engine); };
      std::vector<std::int64_t> local_indices(10);
      std::generate(local_indices.begin(), local_indices.end(), gen);
      std::sort(local_indices.begin(), local_indices.end(),
                [](std::int64_t a, std::int64_t b) { return (a < b); });
      CAPTURE(local_indices);
      CAPTURE(nodes);
      std::vector<t_int> recv = all_to_all_recv_sizes<std::int64_t>(local_indices, nodes, N);
      for (const auto& a : recv) CHECK(a >= 0);
      CHECK(local_indices.size() == std::accumulate(recv.begin(), recv.end(), 0));
    }
  }
}
