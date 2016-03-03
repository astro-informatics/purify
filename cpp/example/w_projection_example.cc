
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

using namespace purify;  

int main( int nargs, char const** args ){
  
  utilities::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_real over_sample;
  t_real cellsize;
  std::string kernel;
  std::string vis_file = "../data/vla/at166B.3C129.c0.vis";
  //std::string vis_file = "../data/ATCA/1637-77.vis";


  //Gridding example
  cellsize = 5;
  over_sample = 2;
  t_int J = 4;
  t_int width = 32;
  t_int height = 32;
  t_int number_of_vis = 20;
  bool use_w_term = true;
  t_real fov = 30;
  uv_vis.u = Vector<t_real>::Random(number_of_vis);
  uv_vis.v = Vector<t_real>::Random(number_of_vis);
  uv_vis.w = Vector<t_real>::Random(number_of_vis);
  uv_vis.weights = Vector<t_complex>::Zero(number_of_vis); uv_vis.weights.setOnes();
  //uv_vis = utilities::set_cell_size(uv_vis, cellsize); // scale uv coordinates to correct pixel size and to units of 2pi
  //uv_vis = utilities::uv_scale(uv_vis, floor(width * over_sample), floor(height * over_sample)); // scale uv coordinates to units of Fourier grid size
  //uv_vis = utilities::uv_symmetry(uv_vis); // Enforce condjugate symmetry by reflecting measurements in uv coordinates

  kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, use_w_term, fov, fov); // Create Measurement Operator
  auto chirp_matrix = op.C;
  for (t_int i = 0; i < chirp_matrix.size(); ++i)
  {
    if (std::abs(chirp_matrix(i)) > 0)
      std::cout << chirp_matrix(i) << '\n';
  }
  
}