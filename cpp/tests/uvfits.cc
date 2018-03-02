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
  const std::string filename = vla_filename("../mwa/uvdump_01.uvfits");
  const auto uv_data = pfitsio::read_uvfits(filename);
}

TEST_CASE("readheader") {

  const std::string filename = vla_filename("../mwa/uvdump_01.uvfits");
  // open fits file
  fitsfile *fptr;
  t_int status = 0;
  fits_open_file(&fptr, filename.c_str(), READONLY, &status);
  t_int hdupos;
  fits_get_hdu_num(fptr, &hdupos);
  CHECK(hdupos == 1);
  // close fits file
  fits_close_file(fptr, &status);
  // report any errors
  if(status) { /* print any error messages */
    fits_report_error(stderr, status);
    throw std::runtime_error("Error reading fits file...");
  }
}
