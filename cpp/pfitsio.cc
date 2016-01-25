#include "pfitsio.h"

namespace purify {
	namespace pfitsio {

	  void write2d(Image<t_real> eigen_image, const std::string& fits_name, const bool& overwrite, const bool& flip) 
	  {
	    /*
	      Writes an image to a fits file.

	      image:: image data, a 2d Image.
	      fits_name:: string contating the file name of the fits file.
	    */
	    if (overwrite == true) {remove(fits_name.c_str());};
	    std::auto_ptr<CCfits::FITS> pFits(0);
	    long naxes[2] = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols())};
	    pFits.reset(new CCfits::FITS(fits_name, FLOAT_IMG, 2, naxes));
	    long fpixel (1);
	    std::vector<long> extAx; 
	    extAx.push_back(naxes[0]);
	    extAx.push_back(naxes[1]);
	    CCfits::ExtHDU* imageExt = pFits->addImage(fits_name, FLOAT_IMG, extAx);
	    if (flip == true)
	    {
	      eigen_image = eigen_image.rowwise().reverse().eval();
	    }
	    eigen_image.resize(naxes[0]*naxes[1], 1);
	    std::valarray<double> image(naxes[0]*naxes[1]);
	    for (int i = 0; i < static_cast<int>(naxes[0]*naxes[1]); ++i)
	    {
	      image[i] = static_cast<float>(eigen_image(i));
	    }
	    imageExt->write(fpixel, naxes[0]*naxes[1], image);
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
	    t_int index;
	    for (t_int i = 0; i < ax1; ++i)
	    {
	      for (t_int j = 0; j < ax2; ++j)
	      {
	        index = utilities::sub2ind(j, i, ax2, ax1);
	        eigen_image(i, j) = contents[index];
	      }
	    }
	    return eigen_image;
	  }
	}
}