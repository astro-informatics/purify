#include "purify/config.h"
#include "catch.hpp"
#include "purify/logging.h"

#include "purify/types.h"

#include "purify/wide_field_utilities.h"

using namespace purify;

TEST_CASE("estimate_sample_density MPI") {
  const t_int imsizex = 1024;
  const t_int imsizey = 1024;
  const t_real cellx = 10;
  const t_real celly = 10;
  const t_real oversample_ratio = 2;
  const t_int M = 6;
  const Vector<t_real> u = Vector<t_real>::Random(M) * 1000;
  const Vector<t_real> v = Vector<t_real>::Random(M) * 1000;
  const sopt::mpi::Communicator comm = sopt::mpi::Communicator::World();
  const Vector<t_complex> weights = widefield::sample_density_weights(
      u, v, cellx, celly, imsizex, imsizey, oversample_ratio, 1, comm);
  REQUIRE(weights.size() == M);
  CHECK(weights.isApprox(Vector<t_complex>::Ones(weights.size())));
}
