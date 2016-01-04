#include "catch.hpp"
#include "MeasurementOperator.h"
#include <iostream>

using namespace purify;
TEST_CASE("Measurement Operator", "[something]") {
  MeasurementOperator op;
  MeasurementOperator::vis_params uv_vis;

  //std::cout << op.pswf(-1,6) << '\n';

  uv_vis = op.read_visibility("../data/vla/at166B.3C129.c0.vis");


  //Degridding example
  t_int over_sample = 2;

  t_real cellsize = 0.3;
  t_real m = 180 * 3600 / cellsize / purify_pi;
  uv_vis.u = uv_vis.u / m * 2 * purify_pi;
  uv_vis.v = uv_vis.v / m * 2 * purify_pi;
  
  Image<t_complex> test_image = op.readfits2d("../data/images/M31.fits");
  uv_vis = op.uv_scale(uv_vis, test_image.cols() * over_sample, test_image.rows() * over_sample);
  uv_vis = op.uv_symmetry(uv_vis);
  MeasurementOperator::operator_params st = op.init_nufft2d(uv_vis.u, uv_vis.v, 4, 4, "gauss", test_image.cols(), test_image.rows(), over_sample);
  Vector<t_complex> new_vis = op.degrid(test_image, st);
  Image<t_complex> new_img = op.grid(new_vis, st);
  op.writefits2d(new_img.real(), "degrid_image_real.fits", true, false); 
  op.writefits2d(new_img.imag(), "degrid_image_imag.fits", true, false); 

  //Gridding example
  over_sample = 2;
  t_int J = 6;
  std::string kernel = "pswf";
  uv_vis = op.read_visibility("../data/vla/at166B.3C129.c0.vis");
  uv_vis = op.uv_scale(uv_vis, 1024 * over_sample, 1024 * over_sample);
  uv_vis = op.uv_symmetry(uv_vis);
  cellsize = 0.3;
  m = 180 * 3600 / cellsize / purify_pi;
  uv_vis.u = uv_vis.u / m * 2 * purify_pi;
  uv_vis.v = uv_vis.v / m * 2 * purify_pi;
  

  st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample);
  new_img = op.grid(uv_vis.vis, st);
  std::stringstream name;
  std::string fname;
  name <<  "grid_image_real_" << kernel << "_" << J << ".fits";
  fname = name.str();
  name.str(std::string());
  op.writefits2d(new_img.real(), fname, true, true);
  name <<  "grid_image_imag_" << kernel << "_" << J << ".fits";
  fname = name.str(); 
  name.str(std::string());
  op.writefits2d(new_img.imag(), fname, true, true);
  name <<  "grid_image_abs_" << kernel << "_" << J << ".fits";
  fname = name.str(); 
  name.str(std::string());
  op.writefits2d(new_img.cwiseAbs(), fname, true, true); 
  op.writefits2d(st.S, "scale.fits", true, true); 
  
  
  SECTION("check one case") {
    CHECK(op.choose_kernel(0, 1, "gauss") == 1);
  }
  /*
  SECTION("check one case") {
    CHECK(op.pswf(0,6) > 0);
  }
  */
}
