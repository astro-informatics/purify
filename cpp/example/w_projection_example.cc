
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify;  
using namespace purify::notinstalled;  


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
  cellsize = 5;//
  over_sample = 2;
  t_int J = 4;
  t_int width = 32;
  t_int height = width;
  t_int number_of_vis = 20;
  bool use_w_term = true;
  
  uv_vis.u = Vector<t_real>::Random(number_of_vis);
  uv_vis.v = Vector<t_real>::Random(number_of_vis);
  uv_vis.w = Vector<t_real>::Random(number_of_vis);
  t_real w_max = uv_vis.w.maxCoeff();
  uv_vis.w = uv_vis.w / w_max * width / 2;
  uv_vis.w = uv_vis.w * 0;
  uv_vis.weights = Vector<t_complex>::Zero(number_of_vis); uv_vis.weights.setOnes();
  //uv_vis = utilities::set_cell_size(uv_vis, cellsize); // scale uv coordinates to correct pixel size and to units of 2pi
  //uv_vis = utilities::uv_scale(uv_vis, floor(width * over_sample), floor(height * over_sample)); // scale uv coordinates to units of Fourier grid size
  //uv_vis = utilities::uv_symmetry(uv_vis); // Enforce condjugate symmetry by reflecting measurements in uv coordinates

  kernel = "kb";
  MeasurementOperator op(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, use_w_term, cellsize, cellsize); // Create Measurement Operator
  auto chirp_matrix = op.C;
  auto first_row = chirp_matrix.row(18);
  MeasurementOperator op0(uv_vis, J, J, kernel, width, height, over_sample, "none", 0, false, cellsize, cellsize); 
//  first_row.resize(64, 64);
//  pfitsio::write2d(first_row.real(), output_filename("first_row.fits"));
  for (t_int i = 0; i < chirp_matrix.rows(); ++i)
  {
    for (t_int j = 0; j < chirp_matrix.cols(); ++j)
    {
      if (std::abs(op0.G.coeffRef(i, j)) > 1e-4)
        std::cout << std::abs(op0.G.coeffRef(i, j)) << " " << std::abs(op.G.coeffRef(i, j)) << " " << i << " " << j << '\n';
    }
  }
  
}