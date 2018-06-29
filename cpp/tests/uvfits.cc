#include "purify/config.h"
#include "catch.hpp"
#include "purify/logging.h"
#include "purify/types.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/uvfits.h"
using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("readfile") {
  purify::logging::set_level("debug");
  const std::string filename = atca_filename("0332-391");
  SECTION("filter") {
    const auto uvfits = pfitsio::read_uvfits(filename + ".uvfits", true, stokes::I);
    const auto vis = utilities::read_visibility(filename + ".vis", true);
    REQUIRE(299792458. == constant::c);
    REQUIRE(1431999959.5 == uvfits.frequencies(0));
    REQUIRE(13 == uvfits.frequencies.size());
    CHECK(uvfits.size() / 22675 == 10);
    CHECK(vis.size() / 22675 == 10);
    CHECK(uvfits.size() == vis.size());
    for(int i = 0; i < uvfits.size(); i++) {
      CAPTURE(i);
      CAPTURE(vis.u(i));
      CAPTURE(uvfits.u(i));
      CAPTURE(vis.v(i));
      CAPTURE(uvfits.v(i));
      CAPTURE(vis.w(i));
      CAPTURE(uvfits.w(i));
      CAPTURE(vis.vis(i));
      CAPTURE(uvfits.vis(i));
      CAPTURE(vis.weights(i));
      CAPTURE(uvfits.weights(i));
      REQUIRE(std::abs((uvfits.u(i) - vis.u(i)) / vis.u(i)) < 1e-4);
      REQUIRE(std::abs((uvfits.v(i) - vis.v(i)) / vis.v(i)) < 1e-4);
      REQUIRE(std::abs((uvfits.w(i) - vis.w(i)) / vis.w(i)) < 1e-4);
      if(std::abs(vis.weights(i)) > 0)
        REQUIRE(std::abs((uvfits.weights(i) - vis.weights(i)) / vis.weights(i)) < 1e-4);
      if(std::abs(uvfits.weights(i)) > 0)
        REQUIRE(std::abs((uvfits.weights(i) - vis.weights(i)) / uvfits.weights(i)) < 1e-4);
      if(std::abs(vis.vis(i)) > 0)
        REQUIRE(std::abs((uvfits.vis(i) - vis.vis(i)) / vis.vis(i)) < 1e-4);
      if(std::abs(uvfits.vis(i)) > 0)
        REQUIRE(std::abs((uvfits.vis(i) - vis.vis(i)) / uvfits.vis(i)) < 1e-4);
    }
  }
  SECTION("no_filter") {
    const auto uvfits = pfitsio::read_uvfits(filename + ".uvfits", false, stokes::I);
    const auto vis = utilities::read_visibility(filename + "_nofilter.vis", true);
    REQUIRE(299792458. == constant::c);
    REQUIRE(1431999959.5 == uvfits.frequencies(0));
    REQUIRE(uvfits.size() == vis.size());
    for(int i = 0; i < uvfits.size(); i++) {
      CAPTURE(i);
      CAPTURE(vis.u(i));
      CAPTURE(uvfits.u(i));
      CAPTURE(vis.v(i));
      CAPTURE(uvfits.v(i));
      CAPTURE(vis.w(i));
      CAPTURE(uvfits.w(i));
      CAPTURE(vis.vis(i));
      CAPTURE(uvfits.vis(i));
      CAPTURE(vis.weights(i));
      CAPTURE(uvfits.weights(i));
      REQUIRE(std::abs((uvfits.u(i) - vis.u(i)) / vis.u(i)) < 1e-4);
      REQUIRE(std::abs((uvfits.v(i) - vis.v(i)) / vis.v(i)) < 1e-4);
      REQUIRE(std::abs((uvfits.w(i) - vis.w(i)) / vis.w(i)) < 1e-4);
    }
  }
}
