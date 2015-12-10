#include "catch.hpp"
#include "MeasurementOperator.h"

using namespace purify;
TEST_CASE("Measurement Operator", "[something]") {
  MeasurementOperator op;

  SECTION("check one case") {
    CHECK(op.something().size() == 0);
    CHECK(op(2).size() == 0);
  }

  SECTION("check another") {
    op.something(Vector<>::Random(5));
    CHECK(op.something().size() == 5);
    CHECK(op(2.1).isApprox(op.something() * 2.1));
  }
}
