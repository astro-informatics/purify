#include "catch.hpp"
#include "MeasurementOperator.h"
#include <iostream>

using namespace purify;
TEST_CASE("Measurement Operator", "[something]") {
  MeasurementOperator op;
  MeasurementOperator::vis_params uv_vis1;
  MeasurementOperator::vis_params uv_vis2;

  //std::cout << op.ft_kaiser_bessel(0, 6) << '\n';

  uv_vis1 = op.read_visibility("../data/vla/at166B.3C129.c0.vis");


  //Degridding example
  t_int over_sample = 2;



  t_real cellsize = 0.3;
  uv_vis1 = op.set_cell_size(uv_vis1, cellsize);
  
  Image<t_complex> test_image = op.readfits2d("../data/images/M31.fits");
  uv_vis1 = op.uv_scale(uv_vis1, test_image.cols() * over_sample, test_image.rows() * over_sample);
  uv_vis1 = op.uv_symmetry(uv_vis1);
  MeasurementOperator::operator_params st1 = op.init_nufft2d(uv_vis1.u, uv_vis1.v, 6, 6, "pswf", test_image.cols(), test_image.rows(), over_sample);
  Vector<t_complex> new_vis = op.degrid(test_image, st1);
  Image<t_complex> new_img = op.grid(new_vis, st1);
  op.writefits2d(new_img.real(), "degrid_image_real.fits", true, false); 
  op.writefits2d(new_img.imag(), "degrid_image_imag.fits", true, false); 
  


  //Gridding example
  over_sample = 2;
  t_int J = 6;
  std::string kernel = "kb";
  uv_vis2 = op.read_visibility("../data/vla/at166B.3C129.c0.vis");
  cellsize = 0.3;
  uv_vis2 = op.set_cell_size(uv_vis2, cellsize);
  uv_vis2 = op.uv_scale(uv_vis2, 1024 * over_sample, 1024 * over_sample);
  uv_vis2 = op.uv_symmetry(uv_vis2);

  

  MeasurementOperator::operator_params st2 = op.init_nufft2d(uv_vis2.u, uv_vis2.v, J, J, kernel, 1024, 1024, over_sample);
  new_img = op.grid(uv_vis2.vis, st2);
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
  op.writefits2d(st2.S, "scale.fits", true, true); 
  
  
  SECTION("check one case") {
    CHECK(op.gaussian(0, 1) == 1);
  }

}
