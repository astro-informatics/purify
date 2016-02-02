#include "catch.hpp"
#include "pfitsio.h"
using namespace purify;

TEST_CASE("Purify fitsio", "[readwrite]") {

  Image<t_complex> input = pfitsio::read2d("../data/images/M31.fits");
  pfitsio::write2d(input.real(), "fits_output.fits");
  Image<t_complex> input2 = pfitsio::read2d("fits_output.fits");
  CHECK(input.isApprox(input2, 1e-13));

}

