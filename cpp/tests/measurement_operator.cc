#include "catch.hpp"
#include "MeasurementOperator.h"
#include <iostream>

using namespace purify;
TEST_CASE("Measurement Operator", "[something]") {
  MeasurementOperator op;
  MeasurementOperator::vis_params uv_vis;
  uv_vis = op.read_visibility("../data/vla/at166B.3C129.c0.vis");
  uv_vis = op.uv_symmetry(uv_vis);

  t_real cellsize = 0.3;
  t_real m = 180*3600/cellsize/pi;
  uv_vis.u = uv_vis.u / m * 2 * pi;
  uv_vis.v = uv_vis.v / m * 2 * pi;
  Image<t_complex> test_image = op.readfits2d("../data/images/M31.fits");
  MeasurementOperator::operator_params st = op.init_nufft2d(uv_vis.u, uv_vis.v, 4, 4, "gauss", test_image.cols(), test_image.rows(), 2);
  Vector<t_complex> new_vis = op.degrid(test_image, st);
  Image<t_complex> new_img = op.grid(new_vis, st);
  op.writefits2d(new_img.real(), "degrid_image_real.fits", true); 
  op.writefits2d(new_img.imag(), "degrid_image_imag.fits", true); 
  st = op.init_nufft2d(uv_vis.u, uv_vis.v, 24, 24, "gauss", 1024, 1024, 2);
  new_img = op.grid(uv_vis.vis, st);
  op.writefits2d(new_img.real(), "grid_image_real.fits", true); 
  op.writefits2d(new_img.imag(), "grid_image_imag.fits", true); 
  op.writefits2d(new_img.cwiseAbs(), "grid_image_abs.fits", true); 
  op.writefits2d(st.S, "scale.fits", true); 
  SECTION("check one case") {
    CHECK(op.choose_kernel(0, 1, "gauss") == 1);
  }
}
