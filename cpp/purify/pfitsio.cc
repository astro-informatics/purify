#include "purify/config.h"
#include "purify/pfitsio.h"

namespace purify {
namespace pfitsio {
//! Write image to fits file
void write2d_header(const Image<t_real> &eigen_image, const pfitsio::header_params &header,
                    const bool &overwrite) {
  /*
    Writes an image to a fits file.

    image:: image data, a 2d Image.
        header:: structure containing header information
        overwrite:: if true, overwrites old fits file with same name

  */

  if(overwrite == true) {
    remove(header.fits_name.c_str());
  };
  long naxes[4]
      = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols()), 1, 1};
  std::unique_ptr<CCfits::FITS> pFits(new CCfits::FITS(header.fits_name, FLOAT_IMG, 4, naxes));
  long fpixel(1);
  std::vector<long> extAx = {eigen_image.rows(), eigen_image.cols()};

  std::valarray<double> image(naxes[0] * naxes[1]);
  std::copy(eigen_image.data(), eigen_image.data() + eigen_image.size(), &image[0]);
  // Writing to fits header
  pFits->pHDU().addKey("AUTHOR", "Purify", "");
  pFits->pHDU().addKey("BUNIT", header.pix_units, ""); // units
  pFits->pHDU().addKey("NAXIS", 4, "");                // number of axes
  pFits->pHDU().addKey("NAXIS1", static_cast<t_int>(eigen_image.cols()), "");
  pFits->pHDU().addKey("NAXIS2", static_cast<t_int>(eigen_image.rows()), "");
  pFits->pHDU().addKey("NAXIS3", 1, "");
  pFits->pHDU().addKey("NAXIS4", 1, "");
  pFits->pHDU().addKey("CRPIX1", static_cast<t_int>(std::floor(eigen_image.cols() / 2)) + 1, "");
  pFits->pHDU().addKey("CRPIX2", static_cast<t_int>(std::floor(eigen_image.rows() / 2)) + 1, "");
  pFits->pHDU().addKey("CRPIX3", 1, "");
  pFits->pHDU().addKey("CRPIX4", 1, "");
  pFits->pHDU().addKey("CRVAL1", header.ra * 180. / purify::constant::pi, "");
  pFits->pHDU().addKey("CRVAL2", header.dec * 180. / purify::constant::pi, "");
  pFits->pHDU().addKey("CRVAL3", header.mean_frequency * std::pow(10, 6) * 1., "");
  pFits->pHDU().addKey("CRVAL4", 1, "");
  pFits->pHDU().addKey("CTYPE1", "RA---SIN", "");
  pFits->pHDU().addKey("CTYPE2", "DEC---SIN", "");
  pFits->pHDU().addKey("CTYPE3", "FREQ-OBS", "");
  pFits->pHDU().addKey("CTYPE4", "STOKES", "");
  pFits->pHDU().addKey("CDELT1", -header.cell_x / 3600., "");
  pFits->pHDU().addKey("CDELT2", header.cell_y / 3600., "");
  pFits->pHDU().addKey("CDELT3", header.channel_width * std::pow(10, 6) * 1., "");
  pFits->pHDU().addKey("CDELT4", 1, "");
  pFits->pHDU().addKey("BTYPE", "intensity", "");
  pFits->pHDU().addKey("EQUINOX", 2000, "");
  pFits->pHDU().addKey("PURIFY-NITERS", header.niters, "");
  if (header.hasconverged){
    pFits->pHDU().addKey("PURIFY-CONVERGED", "T", "");
  } else {
    pFits->pHDU().addKey("PURIFY-CONVERGED", "F", "");
  }
  pFits->pHDU().addKey("PURIFY-RELATIVEVARIATION", header.relative_variation, "");
  pFits->pHDU().addKey("PURIFY-RESIDUALCONVERGENCE", header.residual_convergence, "");
  pFits->pHDU().addKey("PURIFY-EPSILON", header.epsilon, "");

  // Writing image to fits file
  pFits->pHDU().write(fpixel, naxes[0] * naxes[1], image);
}

void write2d(const Image<t_real> &eigen_image, const std::string &fits_name,
             const std::string &pix_units, const bool &overwrite) {
  /*
    Writes an image to a fits file.

    image:: image data, a 2d Image.
    fits_name:: string containing the file name of the fits file.
    pix_units:: units of flux
    ra:: centre pixel coordinate in ra
    dec:: centre pixel coordinate in dec

  */
  if(overwrite == true) {
    remove(fits_name.c_str());
  };
  long naxes[2] = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols())};
  std::unique_ptr<CCfits::FITS> pFits(new CCfits::FITS(fits_name, FLOAT_IMG, 2, naxes));
  long fpixel(1);
  std::vector<long> extAx = {eigen_image.rows(), eigen_image.cols()};

  std::valarray<double> image(naxes[0] * naxes[1]);
  std::copy(eigen_image.data(), eigen_image.data() + eigen_image.size(), &image[0]);
  pFits->pHDU().addKey("AUTHOR", "Purify", "");
  pFits->pHDU().addKey("BUNIT", pix_units, "");
  pFits->pHDU().write(fpixel, naxes[0] * naxes[1], image);
}

Image<t_complex> read2d(const std::string &fits_name) {
  /*
    Reads in an image from a fits file and returns the image.

    fits_name:: name of fits file
  */

  std::auto_ptr<CCfits::FITS> pInfile(new CCfits::FITS(fits_name, CCfits::Read, true));
  std::valarray<t_real> contents;
  CCfits::PHDU &image = pInfile->pHDU();
  image.read(contents);
  t_int ax1(image.axis(0));
  t_int ax2(image.axis(1));
  Image<t_complex> eigen_image(ax1, ax2);
  std::copy(&contents[0], &contents[0] + eigen_image.size(), eigen_image.data());
  return eigen_image;
}
}
}
