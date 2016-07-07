
#include "MeasurementOperator.h"
#include "wprojection.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify;  
using namespace purify::notinstalled;

int main( int nargs, char const** args ){
  //up samples M31 example
    std::string const fitsfile = image_filename("M31.fits");
    auto const M31 = pfitsio::read2d(fitsfile);

    FFTOperator fftop;

    auto ft_grid = fftop.forward(M31);
    auto new_ft_grid = wprojection::re_sample_ft_grid(ft_grid, 4.);
    auto M31_resample = fftop.inverse(new_ft_grid).real();
    auto max = M31_resample.array().maxCoeff();
    M31_resample = M31_resample / max;
    pfitsio::write2d(M31_resample, output_filename("M31_resample_4.fits"));
}