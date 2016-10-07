#include "catch.hpp"
#include "purify/directories.h"
#include "purify/pfitsio.h"
using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("Purify fitsio", "[readwrite]") {

  Image<t_complex> input = pfitsio::read2d(image_filename("M31.fits"));
  pfitsio::write2d(input.real(), output_filename("fits_output.fits"));
  Image<t_complex> input2 = pfitsio::read2d(output_filename("fits_output.fits"));
  CAPTURE(input2);
  CHECK(input.isApprox(input2, 1e-6));
}
