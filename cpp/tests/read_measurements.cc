#include "purify/config.h"
#include "purify/types.h"
#include "catch2/catch_all.hpp"
#include "purify/logging.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/read_measurements.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("mkdir") {
  const std::string dirs = "test/mkdir/recursive";
  const std::string path = output_filename(dirs);
  CAPTURE(path);
  mkdir_recursive(path);
  REQUIRE(read_measurements::dir_exists(path));
}

TEST_CASE("uvfits") {
  purify::logging::set_level("debug");
  const std::string filename = atca_filename("0332-391");
  SECTION("one") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(filename + ".uvfits");
      CHECK(uvfits.size() == 245886);
    }
    SECTION("vis") {
      const auto vis = read_measurements::read_measurements(filename + ".vis", false, stokes::I,
                                                            utilities::vis_units::radians);
      CHECK(vis.units == utilities::vis_units::radians);
      CHECK(vis.size() == 245886);
    }
  }
  SECTION("two") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".uvfits", filename + ".uvfits"});
      CHECK(uvfits.size() == 245886 * 2);
    }
    SECTION("vis") {
      const auto vis = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".vis", filename + ".vis"});
      CHECK(vis.units == utilities::vis_units::lambda);
      CHECK(vis.size() == 245886 * 2);
    }
    SECTION("vis_uvfits") {
      CHECK_THROWS(read_measurements::read_measurements(
          std::vector<std::string>{filename + ".vis", filename + ".uvfits"}));
    }
  }
  SECTION("otherformat") {
    CHECK_THROWS(read_measurements::read_measurements(filename + ".format"));
  }
  SECTION("ms") {
    SECTION("one") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(filename + ".ms");
      CHECK(ms.size() == 245994);
#else
      CHECK_THROWS(read_measurements::read_measurements(filename + ".ms"));
#endif
    }
    SECTION("two") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".ms", filename + ".ms"});
      CHECK(ms.size() == 245994 * 2);
#else
      CHECK_THROWS(read_measurements::read_measurements(
          std::vector<std::string>{filename + ".ms", filename + ".ms"}));
#endif
    }
  }
}
