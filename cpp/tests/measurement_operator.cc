#include "catch.hpp"
#include "MeasurementOperator.h"

using namespace purify;
TEST_CASE("Measurement Operator", "[something]") {
  MeasurementOperator op;

  SECTION("check one case") {
    CHECK(op.gaussian(0, 1) == 1);
  }
}
