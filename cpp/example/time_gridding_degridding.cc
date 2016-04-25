
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"
#include "directories.h"

#include <ctime>

using namespace purify;  
using namespace purify::notinstalled;

int main( int nargs, char const** args ){
  if (nargs != 6 )
  {
    std::cout << " Wrong number of arguments! " << '\n';
    return 1;
  }

  std::string const kernel = args[1];
  t_real const over_sample = std::stod(static_cast<std::string>(args[2]));
  t_int const J = static_cast<t_int>(std::stod(static_cast<std::string>(args[3])));
  t_int const sample = static_cast<t_int>(std::stod(args[4]));
  t_int const number_of_tests = static_cast<t_int>(std::stod(args[5]));  

  std::string const results = output_filename("Grid_Degrid_timing_" + kernel + "_" + std::to_string(sample)+".txt");

  //Gridding example
  t_int const width = 256;
  t_int const height = 256;
  t_int const number_of_vis = 2 * std::floor(width * height * sample / 10.);
  t_real const sigma_m = purify_pi/3;

  Vector<t_real> grid_times = Vector<t_real>::Zero(number_of_tests);
  Vector<t_real> degrid_times = Vector<t_real>::Zero(number_of_tests);

  Image<t_complex> im = Matrix<t_complex>::Random(width, height);
  Vector<t_complex> vis = Vector<t_complex>::Random(number_of_vis);
  t_real inner_loop = 10000;
  t_complex const I(0, 1);
  std::cout.precision(20);
  for (t_int i = 0; i < number_of_tests; ++i)
  {
    auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
    uv_data.units = "radians";
    MeasurementOperator op(uv_data, J, J, kernel, width, height, over_sample); // Generating gridding matrix
    
    uv_data.vis = Vector<t_complex>::Random(number_of_vis) + I*Vector<t_complex>::Random(number_of_vis);
    std::clock_t c_start = std::clock();
    for (t_int j = 0; j < inner_loop; ++j)
    {
      op.grid(uv_data.vis);
    }
    std::clock_t c_end = std::clock();
    grid_times(i) = static_cast<t_real>((c_end - c_start) / CLOCKS_PER_SEC)/inner_loop;
    std::cout << (c_end - c_start) / CLOCKS_PER_SEC/inner_loop << "\n";
  }
  
  for (t_int i = 0; i < number_of_tests; ++i)
  {
    auto uv_data = utilities::random_sample_density(number_of_vis, 0, sigma_m);
    uv_data.units = "radians";
    MeasurementOperator op(uv_data, J, J, kernel, width, height, over_sample); // Generating gridding matrix
    Image<t_complex> im = Matrix<t_complex>::Random(width, height) + I*Matrix<t_complex>::Random(width, height);
    std::clock_t c_start = std::clock();
    for (t_int j = 0; j < inner_loop; ++j)
    {
      op.degrid(im);
    }
    std::clock_t c_end = std::clock();
    degrid_times(i) = static_cast<t_real>((c_end - c_start) / CLOCKS_PER_SEC)/inner_loop;
    std::cout << (c_end - c_start) / CLOCKS_PER_SEC/inner_loop << "\n";
  }  
  std::printf("Done! \n");
  t_real const mean_grid_time = grid_times.array().mean();
  t_real const rms_grid_time = utilities::standard_deviation(grid_times);

  t_real const mean_degrid_time = degrid_times.array().mean();
  t_real const rms_degrid_time = utilities::standard_deviation(degrid_times);
  //writing snr and time to a file
  std::ofstream out(results);
  out.precision(20);
  out << mean_grid_time << " " << rms_grid_time << " " << mean_degrid_time << " " << rms_degrid_time << "\n";
  std::cout << mean_grid_time << " " << rms_grid_time << " " << mean_degrid_time << " " << rms_degrid_time << "\n";
  out.close();
}