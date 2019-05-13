#include "catch.hpp"

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"

#include "purify/directories.h"
#include "purify/pfitsio.h"

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("Purify fitsio", "[readwrite]") {
  Image<t_complex> input = pfitsio::read2d(image_filename("M31.fits"));
  pfitsio::write2d(input.real(), output_filename("fits_output.fits"));
  Image<t_complex> input2 = pfitsio::read2d(output_filename("fits_output.fits"));
  CAPTURE(input2);
  CHECK(input.isApprox(input2, 1e-12));
}

TEST_CASE("readwrite2dheader", "purify fitsio") {
  Image<t_complex> input = pfitsio::read2d(image_filename("M31.fits"));
  pfitsio::header_params header_example;
  header_example.fits_name = output_filename("fits_output.fits");
  pfitsio::write2d(input.real(), header_example);
  Image<t_complex> input2 = pfitsio::read2d(output_filename("fits_output.fits"));
  CHECK(input.isApprox(input2, 1e-12));
}

TEST_CASE("readwrite3d", "purify fitsio") {
  purify::logging::set_level("debug");
  std::vector<Image<t_complex>> input = pfitsio::read3d(image_filename("cube_example.fits"));
  CHECK(input.size() == 4);
  CHECK(input[0].size() == 200 * 200);
  std::vector<Image<t_real>> input_real;
  for (int i = 0; i < input.size(); i++) {
    input_real.push_back(input[i].real());
  }
  pfitsio::write3d(input_real, output_filename("cube_output.fits"));
  std::vector<Image<t_complex>> input2 = pfitsio::read3d(output_filename("cube_output.fits"));
  for (int i = 0; i < input.size(); i++) {
    CHECK(input[i].isApprox(input2[i], 1e-12));
  }
}
TEST_CASE("readwrite3dheader", "purify fitsio") {
  std::vector<Image<t_complex>> input = pfitsio::read3d(image_filename("cube_example.fits"));
  CHECK(input.size() == 4);
  CHECK(input.at(0).size() == 200 * 200);
  std::vector<Image<t_real>> input_real;
  for (int i = 0; i < input.size(); i++) {
    CAPTURE(input.size());
    CAPTURE(i);
    input_real.push_back(input.at(i).real());
  }
  pfitsio::header_params header_example;
  header_example.fits_name = output_filename("cube_output.fits");
  pfitsio::write3d(input_real, header_example);
  std::vector<Image<t_complex>> input2 = pfitsio::read3d(output_filename("cube_output.fits"));
  CAPTURE(input.size());
  for (int i = 0; i < input.size(); i++) {
    CAPTURE(i);
    CHECK(input.at(i).isApprox(input2.at(i), 1e-12));
  }
}
TEST_CASE("readwrite3dwith2d", "purify fitsio") {
  std::vector<Image<t_complex>> input = pfitsio::read3d(image_filename("M31.fits"));
  CHECK(input.size() == 1);
  CHECK(input[0].size() == 256 * 256);
  std::vector<Image<t_real>> input_real;
  for (int i = 0; i < input.size(); i++) {
    input_real.push_back(input[i].real());
  }
  pfitsio::write3d(input_real, output_filename("2dcube_output.fits"));
  std::vector<Image<t_complex>> input2 = pfitsio::read3d(output_filename("2dcube_output.fits"));
  for (int i = 0; i < input.size(); i++) CHECK(input[i].isApprox(input2[i], 1e-12));
}
TEST_CASE("readwrite3dheaderwith2d", "purify fitsio") {
  std::vector<Image<t_complex>> input = pfitsio::read3d(image_filename("M31.fits"));
  CHECK(input.size() == 1);
  CHECK(input[0].size() == 256 * 256);
  std::vector<Image<t_real>> input_real;
  for (int i = 0; i < input.size(); i++) input_real.push_back(input[i].real());

  pfitsio::header_params header_example;
  header_example.fits_name = output_filename("2dcube_output.fits");
  pfitsio::write3d(input_real, header_example);
  std::vector<Image<t_complex>> input2 = pfitsio::read3d(output_filename("2dcube_output.fits"));
  for (int i = 0; i < input.size(); i++) CHECK(input[i].isApprox(input2[i], 1e-12));
}

TEST_CASE("header") {
  const std::string fits_name = "test_image.fits";
  const t_real mean_frequency = 123;  // in MHz
  const t_real cell_x = 5;            // in arcseconds
  const t_real cell_y = 2;            // in arcseconds
  const t_real ra = 12;               // in radians, converted to decimal degrees before write
  const t_real dec = 98;              // in radians, converted to decimal degrees before write
  const std::string pix_units = "Jy/PIXEL";
  const t_real channels_total = 2;
  const t_real channel_width = 11;  // in MHz
  const t_real polarisation = stokes_int.at(stokes::I);
  const int niters = 10;           // number of iterations
  const bool hasconverged = true;  // stating if model has converged
  const t_real relative_variation = 1e-3;
  const t_real residual_convergence = 1e-4;
  const t_real epsilon = 10;
  pfitsio::header_params header;
  header.fits_name = fits_name;
  header.mean_frequency = mean_frequency;
  header.cell_x = cell_x;
  header.cell_y = cell_y;
  header.ra = ra;
  header.dec = dec;
  header.pix_units = pix_units;
  header.channels_total = channels_total;
  header.channel_width = channel_width;
  header.polarisation = polarisation;
  header.niters = niters;
  header.hasconverged = hasconverged;
  header.relative_variation = relative_variation;
  header.residual_convergence = residual_convergence;
  header.epsilon = epsilon;
  const auto header_test = pfitsio::header_params(
      fits_name, pix_units, channels_total, ra, dec, stokes::I, cell_x, cell_y, mean_frequency,
      channel_width, niters, hasconverged, relative_variation, residual_convergence, epsilon);
  CHECK(header_test == header);
  CHECK(header_test == pfitsio::header_params(fits_name, pix_units, channels_total, ra, dec,
                                              stokes_string.at("p"), cell_x, cell_y, mean_frequency,
                                              channel_width, niters, hasconverged,
                                              relative_variation, residual_convergence, epsilon));
  CHECK_THROWS(header_test == pfitsio::header_params(fits_name, pix_units, channels_total, ra, dec,
                                                     stokes_string.at("z"), cell_x, cell_y,
                                                     mean_frequency, channel_width, niters,
                                                     hasconverged, relative_variation,
                                                     residual_convergence, epsilon));
  CHECK(header_test != pfitsio::header_params(fits_name, pix_units, channels_total, ra, dec,
                                              stokes::Q, cell_x, cell_y, mean_frequency,
                                              channel_width, niters, hasconverged,
                                              relative_variation, residual_convergence, epsilon));
}
