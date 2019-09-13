#include "purify/config.h"
#include <iostream>
#include "catch.hpp"
#include "purify/logging.h"

#include "purify/types.h"

#include "purify/convolution.h"
#include "purify/directories.h"
#include "purify/kernels.h"
#include "purify/uvw_utilities.h"
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
    CHECK(
        1. ==
        Approx(widefield::equivalent_miriad_cell_size(1., imsizex, oversample_ratio)).margin(1e-4));
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
    CHECK(3. == Approx(widefield::equivalent_miriad_cell_size(cell, imsizex, oversample_ratio))
                    .margin(1e-4));
  }
}

TEST_CASE("test cell size conversion") {
  const t_real oversample_ratio = 2;
  const t_int imsize = 8192;
  for (t_real FoV : {0.1, 0.2, 0.3, 0.4, 0.5, 1., 5., 10., 15., 20., 25., 30., 40., 50., 60., 70.,
                     80., 90., 120.}) {
    const t_real cell = FoV / imsize * 60 * 60;
    const t_real miriad_cell =
        widefield::equivalent_miriad_cell_size(cell, imsize, oversample_ratio);
    CAPTURE(cell);
    CAPTURE(miriad_cell);
    CAPTURE(FoV);
    CAPTURE(miriad_cell * imsize / 60. / 60.);
    CAPTURE((1 - miriad_cell * imsize / 60. / 60. / FoV) * FoV);
    if (FoV < 0.5)
      CHECK(cell == Approx(miriad_cell).margin(1e-12));
    else
      CHECK(cell > miriad_cell);
  }
}
TEST_CASE("calculate du given a dl and image size") {
  const t_real oversample_ratio = 2;
  const t_int imsize = 128;
  const t_real dl = 0.01;
  // show that upsampling image does not change resolution of fourier grid
  for (t_real oversample_ratio_image_domain : {1, 2, 3, 5})
    CHECK(widefield::dl2du(dl, imsize, oversample_ratio) ==
          Approx(widefield::dl2du(dl / oversample_ratio_image_domain,
                                  std::floor(imsize * oversample_ratio_image_domain),
                                  oversample_ratio))
              .epsilon(1e-12));
  CHECK(dl == Approx(widefield::du2dl(widefield::dl2du(dl, imsize, oversample_ratio), imsize,
                                      oversample_ratio))
                  .epsilon(1e-12));
}

TEST_CASE("Calcuate DDE Image") {
  const t_int imsizex = 128;
  const t_int imsizey = 128;
  const t_real dl = 0.001;
  const t_real dm = 0.001;
  SECTION("w is zero") {
    const t_real w_rate = 0;
    const Image<t_complex> chirp_image =
        widefield::generate_dde([](t_real, t_real) { return 1.; }, dl, dm, imsizex, imsizey, 0);
    REQUIRE(chirp_image.cols() == imsizex);
    REQUIRE(chirp_image.rows() == imsizey);
    REQUIRE(chirp_image.isApprox(Image<t_complex>::Constant(imsizey, imsizex, 1)));
  }
}

TEST_CASE("estimate_sample_density") {
  const t_int imsizex = 1024;
  const t_int imsizey = 1024;
  const t_real cellx = 10;
  const t_real celly = 10;
  const t_real oversample_ratio = 2;
  const t_int M = 6;
  const Vector<t_real> u = Vector<t_real>::Random(M) * 1000;
  const Vector<t_real> v = Vector<t_real>::Random(M) * 1000;

  const Vector<t_complex> weights =
      widefield::sample_density_weights(u, v, cellx, celly, imsizex, imsizey, oversample_ratio, 1);
  REQUIRE(weights.size() == M);
  CHECK(weights.isApprox(Vector<t_complex>::Ones(weights.size())));
}
