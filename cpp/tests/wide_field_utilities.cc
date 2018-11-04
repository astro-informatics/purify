#include "purify/config.h"
#include <iostream>
#include "catch.hpp"
#include "purify/logging.h"

#include "purify/types.h"

#include "purify/convolution.h"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/utilities.h"
#include "purify/wide_field_utilities.h"


using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("uvw units") {
  const t_uint imsizex = 128;
  const t_uint imsizey = 128;
  const t_real oversample_ratio = 2;

  SECTION("1arcsec") {
    const utilities::vis_params uv_lambda(Vector<t_real>::Ones(5), Vector<t_real>::Ones(5),
                                          Vector<t_real>::Ones(5), Vector<t_complex>::Ones(5),
                                          Vector<t_complex>::Ones(5));
    auto const uv_radians = utilities::set_cell_size(uv_lambda, 1., 1.);
    auto const uv_pixels = utilities::uv_scale(uv_radians, std::floor(oversample_ratio * imsizex),
                                               std::floor(oversample_ratio * imsizey));
    CHECK(uv_radians.units == utilities::vis_units::radians);
    CHECK(uv_lambda.units == utilities::vis_units::lambda);
    CHECK(uv_pixels.units == utilities::vis_units::pixels);
    // const t_real scale = 60. * 60. * 180. / std::floor(oversample_ratio * imsizex) /
    // constant::pi;
    const t_real scale = widefield::pixel_to_lambda(1., imsizex, oversample_ratio);
    CAPTURE(1. / scale);
    CAPTURE(uv_pixels.u.transpose());
    CHECK(uv_lambda.u.isApprox(uv_pixels.u * scale, 1e-6));
  }
  SECTION("arcsec") {
    const t_real cell = 3;
    const utilities::vis_params uv_lambda(Vector<t_real>::Ones(5), Vector<t_real>::Ones(5),
                                          Vector<t_real>::Ones(5), Vector<t_complex>::Ones(5),
                                          Vector<t_complex>::Ones(5));
    auto const uv_radians = utilities::set_cell_size(uv_lambda, cell, cell);
    auto const uv_pixels = utilities::uv_scale(uv_radians, std::floor(oversample_ratio * imsizex),
                                               std::floor(oversample_ratio * imsizey));
    CHECK(uv_radians.units == utilities::vis_units::radians);
    CHECK(uv_lambda.units == utilities::vis_units::lambda);
    CHECK(uv_pixels.units == utilities::vis_units::pixels);
    // const t_real scale
    //    = 60. * 60. * 180. / cell / std::floor(oversample_ratio * imsizex) / constant::pi;
    const t_real scale = widefield::pixel_to_lambda(cell, imsizex, oversample_ratio);
    CAPTURE(1. / scale);
    CAPTURE(uv_pixels.u.transpose());
    CHECK(uv_lambda.u.isApprox(uv_pixels.u * scale, 1e-6));
  }
}

TEST_CASE("Calcuate DDE Image") {
  const t_int imsizex = 128;
  const t_int imsizey = 128;
  SECTION("w is zero") {
    const t_real w_rate = 0;
    const Image<t_complex> chirp_image =
        widefield::generate_dde([](t_real, t_real) { return 1.; }, 1, 1, imsizex, imsizey);
    REQUIRE(chirp_image.cols() == imsizex);
    REQUIRE(chirp_image.rows() == imsizey);
    REQUIRE(chirp_image.isApprox(Image<t_complex>::Constant(imsizey, imsizex, 1)));
  }
}
