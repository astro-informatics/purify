#include "purify/config.h"
#include "catch.hpp"
#include "purify/logging.h"
#include "purify/types.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/read_measurements.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("uvfits") {
  purify::logging::set_level("debug");
  const std::string filename = atca_filename("0332-391");
  SECTION("one"){
    SECTION("uvfits") {
     const auto uvfits = read_measurements::read_measurements(filename + ".uvfits");
    }
    SECTION("vis"){
     const auto vis = read_measurements::read_measurements(filename + ".vis");
    }
  }
  SECTION("two"){
    SECTION("uvfits") {
     const auto uvfits = read_measurements::read_measurements(std::vector<std::string>{filename + ".uvfits", filename + ".uvfits"});
    }
    SECTION("vis"){
     const auto vis = read_measurements::read_measurements(std::vector<std::string>{filename + ".vis", filename + ".vis"});
    }
    SECTION("vis_uvfits"){
    CHECK_THROWS(read_measurements::read_measurements(std::vector<std::string>{filename + ".vis", filename + ".uvfits"}));
    }
  }
    SECTION("otherformat") {
     CHECK_THROWS(read_measurements::read_measurements(filename + ".format"));
    }
}

TEST_CASE("ms") {
  purify::logging::set_level("debug");
  const std::string filename = atca_filename("0332-391");
  SECTION("one"){
#ifdef PURIFY_CASACORE
     const auto ms = read_measurements::read_measurements(filename + ".ms");
#else
     CHECK_THROWS(read_measurements::read_measurements(filename + ".ms"));
#endif
  }
  SECTION("two"){
#ifdef PURIFY_CASACORE
     const auto uvfits = read_measurements::read_measurements(std::vector<std::string>{filename + ".ms", filename + ".ms"});
#else
     CHECK_THROWS(read_measurements::read_measurements(std::vector<std::string>{filename + ".ms", filename + ".ms"}));
#endif
    }
}
