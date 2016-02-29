#include <CCfits/CCfits>
#include "pfitsio.h"

namespace purify {
	namespace pfitsio {

	  void write2d(const Image<t_real>& eigen_image, const std::string& fits_name, const bool& overwrite, const bool& flip) 
	  {
	    if (flip == true)
	    {
	      write2d(eigen_image.rowwise().reverse(), fits_name, overwrite, false);
	      return;
	    }
	    /*
	      Writes an image to a fits file.

	      image:: image data, a 2d Image.
	      fits_name:: string contating the file name of the fits file.
	    */
	    if (overwrite == true) {remove(fits_name.c_str());};
	    long naxes[2] = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols())};
	    std::unique_ptr<CCfits::FITS> pFits(new CCfits::FITS(fits_name, FLOAT_IMG, 2, naxes));
	    long fpixel (1);
	    std::vector<long> extAx = {eigen_image.rows(), eigen_image.cols()};
	    
	    std::valarray<double> image(naxes[0]*naxes[1]);
	    std::copy(eigen_image.data(), eigen_image.data() + eigen_image.size(), &image[0]);
	    pFits->pHDU().addKey("AUTHOR", "Purify", "");
	    pFits->pHDU().write ( fpixel, naxes[0]*naxes[1], image);
	  }
	  
	  Image<t_complex> read2d(const std::string& fits_name)
	  {
	    /*
	      Reads in an image from a fits file and returns the image.

	      fits_name:: name of fits file
	    */

	    std::auto_ptr<CCfits::FITS> pInfile(new CCfits::FITS(fits_name, CCfits::Read, true));
	    std::valarray<t_real>  contents;
	    CCfits::PHDU& image = pInfile->pHDU();
	    image.read(contents);
	    t_int ax1(image.axis(0));
	    t_int ax2(image.axis(1));
	    Image<t_complex> eigen_image(ax1, ax2);
	    std::copy(&contents[0], &contents[0] + eigen_image.size(), eigen_image.data());
	    return eigen_image;
	  }
	}
}
