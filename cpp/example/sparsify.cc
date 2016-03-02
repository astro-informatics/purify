
#include "MeasurementOperator.h"
#include "utilities.h"
#include "pfitsio.h"

#include "directories.h"

using namespace purify; 
using namespace purify::notinstalled; 

int main( int nargs, char const** args ){
    //sparsifies M31 example
    std::string const fitsfile = image_filename("M31.fits");
    auto const M31 = pfitsio::read2d(fitsfile);

    FFTOperator fftop;

    auto ft_grid = fftop.inverse(M31);
    auto new_ft_grid = utilities::sparsify_chirp(ft_grid, 0.8);
    auto M31_sparse = fftop.forward(new_ft_grid).real();
    pfitsio::write2d(M31_sparse, output_filename("sparse_m31.fits"));
}