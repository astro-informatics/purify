#include "purify/config.h"
#include "purify/types.h"
#include "catch2/catch_all.hpp"
#include "purify/logging.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/read_measurements.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("uvfits") {
  auto const comm = sopt::mpi::Communicator::World();
  const std::string filename = atca_filename("0332-391");
  SECTION("one") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(filename + ".uvfits", comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886);
    }
    SECTION("vis") {
      const auto vis =
          read_measurements::read_measurements(filename + ".vis", comm, distribute::plan::w_term,
                                               false, stokes::I, utilities::vis_units::radians);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(vis.size()) == 245886);
      CHECK(vis.units == utilities::vis_units::radians);
    }
  }
  SECTION("two") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".uvfits", filename + ".uvfits"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886 * 2);
    }
    SECTION("vis") {
      const auto vis = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".vis", filename + ".vis"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(vis.size()) == 245886 * 2);
      CHECK(vis.units == utilities::vis_units::lambda);
    }
  }
  SECTION("ms") {
    SECTION("one") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(filename + ".ms", comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(ms.size()) == 245994);
#endif
    }
    SECTION("two") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".ms", filename + ".ms"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(ms.size()) == 245994 * 2);
#endif
    }
  }
}
