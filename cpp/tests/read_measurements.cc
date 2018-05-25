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
     const auto uvfits = read_measurements::read_measurements({filename + ".uvfits", filename + ".uvfits"});
    }
    SECTION("vis"){
     const auto vis = read_measurements::read_measurements({filename + ".vis", filename + ".vis"});
    }
    SECTION("vis_uvfits"){
    CHECK_THROWS(read_measurements::read_measurements({filename + ".vis", filename + ".uvfits"}));
    }
  }
}
