
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify;  
using namespace purify::notinstalled;

int main( int nargs, char const** args ){
  //up samples M31 example
    std::string const fitsfile = image_filename("M31.fits");
    auto const M31 = pfitsio::read2d(fitsfile);

    FFTOperator fftop;

    auto ft_grid = fftop.inverse(M31);
    auto new_ft_grid = utilities::re_sample_ft_grid(ft_grid, 2);
    new_ft_grid = utilities::re_sample_ft_grid(new_ft_grid, 1/2.);
    auto M31_resample = fftop.forward(new_ft_grid).real();
    pfitsio::write2d(M31_resample, output_filename("re_sample_m31.fits"));
}