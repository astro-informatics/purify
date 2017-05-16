#include "purify/config.h"
#include <numeric>
#include <random>
#include "catch.hpp"
#include "purify/IndexMapping.h"
#include "purify/types.h"
#include "purify/utilities.h"

using namespace purify;

TEST_CASE("Index mapping operator") {
  SECTION("No overlap") {
    std::vector<t_int> indices{1, 3, 5, 2};
    Vector<t_int> input(10);
    std::iota(input.data(), input.data() + input.size(), 0);

    auto mapper = IndexMapping(indices, input.size());
    Vector<t_int> output;

    mapper(input, output);
    SECTION("Direct application") {
      CHECK(output.size() == indices.size());
      decltype(output)::Index i(0);
      for(auto const &index : indices)
        CHECK(output(i++) == index);
    }

    Vector<t_int> adjoint;
    mapper.adjoint(output, adjoint);
    SECTION("Adjoint application") {
      CHECK(adjoint.size() == input.size());
      for(auto const &index : indices) {
        CHECK(adjoint(index) == index);
        adjoint(index) = 0;
      }
      CHECK(adjoint == adjoint.Zero(adjoint.size()));
    }
  }
  SECTION("With overlap") {
    std::vector<t_int> indices{1, 2, 5, 2};
    Vector<t_int> input(10);
    std::iota(input.data(), input.data() + input.size(), 0);

    auto mapper = IndexMapping(indices, input.size());
    Vector<t_int> output;

    mapper(input, output);
    SECTION("Direct application") {
      CHECK(output.size() == indices.size());
      decltype(output)::Index i(0);
      for(auto const &index : indices)
        CHECK(output(i++) == index);
    }

    SECTION("Adjoint application") {
      Vector<t_int> adjoint;
      mapper.adjoint(output, adjoint);
      CHECK(adjoint.size() == input.size());
      for(auto const &index : std::set<t_int>(indices.begin(), indices.end())) {
        CHECK(adjoint(index) == (index == 2 ? 2 * index : index));
        adjoint(index) = 0;
      }
      CHECK(adjoint == adjoint.Zero(adjoint.size()));
    }
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

TEST_CASE("Shrink sparse matrix") {
  Sparse<t_int> matrix(4, 4);
  std::vector<Eigen::Triplet<t_int>> const triplets = {{0, 0, 1}, {0, 3, 2}, {2, 3, 3}};
  matrix.setFromTriplets(triplets.begin(), triplets.end());
  Sparse<t_int> shrinked_matrix = compress_outer(matrix);

  CHECK(shrinked_matrix.nonZeros() == matrix.nonZeros());
  CHECK(shrinked_matrix.coeffRef(0, 0) == matrix.coeffRef(0, 0));
  CHECK(shrinked_matrix.coeffRef(0, 1) == matrix.coeffRef(0, 3));
  CHECK(shrinked_matrix.coeffRef(2, 1) == matrix.coeffRef(2, 3));
}

TEST_CASE("Vector-shrinked matrix multiplication") {
  auto const N = 10;
  extern std::unique_ptr<std::mt19937_64> mersenne;
  std::uniform_int_distribution<sopt::t_int> uniform_dist(0, N - 1);
  Matrix<t_int> matrix = (Image<uint8_t>::Random(N, N) < uint8_t(64))
                             .matrix()
                             .select(Matrix<uint8_t>::Random(N, N), Matrix<uint8_t>::Zero(N, N))
                             .cast<t_int>();

  matrix.row(uniform_dist(*mersenne)).fill(0);
  matrix.row(uniform_dist(*mersenne)).fill(0);
  Sparse<t_int> const sparse = matrix.sparseView();
  Vector<t_int> const input = Vector<uint8_t>::Random(N).cast<t_int>();

  auto const compressed = compress_outer(sparse);
  auto const mapper = IndexMapping(non_empty_outers(sparse), input.size());
  Vector<t_int> comp_input;
  mapper(input, comp_input);

  CHECK(sparse * input == compressed * comp_input);
  CHECK(utilities::sparse_multiply_matrix(sparse, input) == compressed * comp_input);
  CHECK(utilities::sparse_multiply_matrix(compressed, comp_input) == compressed * comp_input);
}
