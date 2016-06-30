
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

using namespace purify;  

int main( int nargs, char const** args ){
  /*
    This is used to profile the gridding operator
  */

  if (nargs != 5 )
  {
    std::cout << " Wrong number of arguments! " << '\n';
    return 1;
  }
  std::string const kernel = args[1];
  auto const over_sample = std::stod(static_cast<std::string>(args[2]));
  auto const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  auto const M_N_ratio = std::stod(static_cast<std::string>(args[4]));

  
  t_int width = 256;
  t_int height = width;

  t_int const number_of_vis = std::floor(M_N_ratio * width * height);
  t_real const sigma_m = purify_pi/3;
  
  auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
  uv_data.units = "radians";


  MeasurementOperator op(uv_data, J, J, kernel, width, height, 20, over_sample); // Generating gridding matrix

  op.grid(uv_data.vis);

}