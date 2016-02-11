
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

  if (nargs != 3 )
  {
    std::cout << " Wrong number of arguments! " << '\n';
    return 1;
  }
  kernel = args[1];
  over_sample = std::stod(static_cast<std::string>(args[2]));

  //Gridding example
  t_int vis_num = 1e6;
  t_int J = 6;
  t_int width = 4096;
  t_int height = 4096;
  uv_vis.u = Vector<t_real>::LinSpaced(vis_num, 0, floor(over_sample * width / 3.));
  uv_vis.v = Vector<t_real>::LinSpaced(vis_num, 0, floor(over_sample * height / 3.));
  uv_vis.weights = uv_vis.u.array()*static_cast<t_complex>(0) + 1.;
  uv_vis.vis = uv_vis.u.array()*static_cast<t_complex>(0) + 1.;


  MeasurementOperator op(uv_vis.u, uv_vis.v, uv_vis.weights, J, J, kernel, width, height, over_sample); // Generating gridding matrix
 
  //Vector<t_complex> point_source = uv_vis.vis * 0; point_source.setOnes();  // Creating model visibilities for point source
  //Image<t_complex> psf;
  //psf = op.grid(point_source);
  //max = psf.real().maxCoeff();
  //psf = psf / max;
  //pfitsio::write2d(psf.real(), "kb_psf.fits", true, false);

  Image<t_real> kb_img = op.grid(uv_vis.vis).real();
  //max = kb_img.maxCoeff();
  //kb_img = kb_img / max;
  //pfitsio::write2d(kb_img.real(), "grid_image_real_kb_4.fits", true, false);
  //pfitsio::write2d(op.S.real(), "scale_kb_4.fits", true, false);
}