#include "purify/config.h"
#include "purify/logging.h"

#include "purify/pfitsio.h"
namespace purify {
namespace pfitsio {
void write_key(fitsfile *fptr, const std::string &key, const std::string &value,
               const std::string &comment, int *status) {
  std::string entry = value;
  if(fits_write_key(fptr, TSTRING, const_cast<char *>(key.c_str()),
                    reinterpret_cast<void *>(const_cast<char *>(entry.c_str())),
                    const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Problem writing key in fits file: " + key);
}
void write_key(fitsfile *fptr, const std::string &key, const char *value,
               const std::string &comment, int *status) {
  write_key(fptr, key, std::string(value), comment, status);
}
void write_history(fitsfile *fptr, const std::string &context, const std::string &history,
                   int *status) {
  const std::string entry = context + ": " + history;
  if(fits_write_history(fptr, const_cast<char *>(entry.c_str()), status))
    throw std::runtime_error("Problem writing comments in fits file");
}
std::string read_key(fitsfile *fptr, const std::string &key, int *status) {

  std::string value = "";
  std::string comment = "";
  if(fits_read_key(fptr, TSTRING, const_cast<char *>(key.c_str()),
                   const_cast<char *>(value.c_str()), const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Error reading value from key " + key);
  return value;
}
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
  if(header.hasconverged) {
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
  PURIFY_LOW_LOG("Writing fits file {}", fits_name);
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
void write3d_header(const std::vector<Image<t_real>> &eigen_images,
                    const pfitsio::header_params &header, const bool &overwrite) {
  /*
     Writes an image to a fits file.
     image:: image data, a 2d Image.
     header:: structure containing header information
     overwrite:: if true, overwrites old fits file with same name
*/

  PURIFY_LOW_LOG("Writing fits file {}", header.fits_name);
  if(overwrite == true) {
    remove(header.fits_name.c_str());
  }
  fitsfile *fptr;
  t_int status = 0;
  std::vector<long> naxes
      = {static_cast<long>(eigen_images.at(0).rows()), static_cast<long>(eigen_images.at(0).cols()),
         static_cast<long>(eigen_images.size()), 1};
  std::vector<long> fpixel = {1, 1, 1, 1};

  Vector<t_real> image = Vector<t_real>::Zero(naxes.at(0) * naxes.at(1) * naxes.at(2));
  for(int i = 0; i < eigen_images.size(); i++)
    image.segment(i * naxes.at(0) * naxes.at(1), naxes.at(0) * naxes.at(1))
        = Vector<t_real>::Map(eigen_images.at(i).data(), naxes.at(0) * naxes.at(1));

  if(fits_create_file(&fptr, header.fits_name.c_str(), &status))
    throw std::runtime_error("Problem creating fits file:" + header.fits_name);
  if(fits_create_img(fptr, DOUBLE_IMG, static_cast<t_int>(naxes.size()), naxes.data(), &status))
    throw std::runtime_error("Problem creating HDU for image in fits file:" + header.fits_name);
  if(fits_write_pix(fptr, TDOUBLE, fpixel.data(), static_cast<long>(image.size()),
                    const_cast<t_real *>(image.data()), &status))
    throw std::runtime_error("Problem writing image in fits file:" + header.fits_name);

#define PURIFY_MACRO(KEY, VALUE, COMMENT) write_key(fptr, KEY, VALUE, COMMENT, &status);

  // Writing to fits header
  PURIFY_MACRO("BUNIT", header.pix_units, "");                 // units
  PURIFY_MACRO("NAXIS", static_cast<t_int>(naxes.size()), ""); // number of axes
  PURIFY_MACRO("NAXIS1", static_cast<t_int>(naxes.at(0)), "");
  PURIFY_MACRO("NAXIS2", static_cast<t_int>(naxes.at(1)), "");
  PURIFY_MACRO("NAXIS3", static_cast<t_int>(naxes.at(2)), "");
  PURIFY_MACRO("NAXIS4", static_cast<t_int>(naxes.at(3)), "");
  PURIFY_MACRO("CRPIX1", static_cast<t_int>(std::floor(naxes.at(0) / 2)) + 1, "");
  PURIFY_MACRO("CRPIX2", static_cast<t_int>(std::floor(naxes.at(1) / 2)) + 1, "");
  PURIFY_MACRO("CRPIX3", 1, "");
  PURIFY_MACRO("CRPIX4", 1, "");
  PURIFY_MACRO("CRVAL1", static_cast<t_real>(header.ra * 180. / purify::constant::pi), "");
  PURIFY_MACRO("CRVAL2", static_cast<t_real>(header.dec * 180. / purify::constant::pi), "");
  PURIFY_MACRO("CRVAL3", static_cast<t_real>(header.mean_frequency * std::pow(10, 6) * 1.), "");
  PURIFY_MACRO("CRVAL4", static_cast<t_real>(1), "");
  PURIFY_MACRO("CTYPE1", "RA---SIN", "");
  PURIFY_MACRO("CTYPE2", "DEC---SIN", "");
  PURIFY_MACRO("CTYPE3", "FREQ-OBS", "");
  PURIFY_MACRO("CTYPE4", "STOKES", "");
  PURIFY_MACRO("CDELT1", static_cast<t_real>(-header.cell_x / 3600.), "");
  PURIFY_MACRO("CDELT2", static_cast<t_real>(header.cell_y / 3600.), "");
  PURIFY_MACRO("CDELT3", static_cast<t_real>(header.channel_width * std::pow(10, 6) * 1.), "");
  PURIFY_MACRO("CDELT4", 1., "");
  PURIFY_MACRO("BTYPE", "intensity", "");
  PURIFY_MACRO("EQUINOX", 2000, "");

#undef PURIFY_MACRO
  write_history(fptr, "SOFTWARE", "Purify", &status);
  write_history(fptr, "PURIFY-NITERS", header.niters, &status);
  if(header.hasconverged) {
    write_history(fptr, "PURIFY-CONVERGED", "True", &status);
  } else {
    write_history(fptr, "PURIFY-CONVERGED", "False", &status);
  }
  write_history(fptr, "PURIFY-RELATIVEVARIATION", header.relative_variation, &status);
  write_history(fptr, "PURIFY-RESIDUALCONVERGENCE", header.residual_convergence, &status);
  write_history(fptr, "PURIFY-EPSILON", header.epsilon, &status);
  if(fits_write_date(fptr, &status))
    throw std::runtime_error("Problem writing date in fits file:" + header.fits_name);

  if(fits_close_file(fptr, &status))
    throw std::runtime_error("Problem closing fits file:" + header.fits_name);
}

void write3d(const std::vector<Image<t_real>> &eigen_images, const std::string &fits_name,
             const std::string &pix_units, const bool &overwrite) {
  /*
     Writes a vector of images to a fits file.
     image:: image data, a 3d Image.
     fits_name:: string containing the file name of the fits file.
     pix_units:: units of flux
     ra:: centre pixel coordinate in ra
     dec:: centre pixel coordinate in dec
*/
  if(overwrite == true) {
    remove(fits_name.c_str());
  };
  long naxes[3]
      = {static_cast<long>(eigen_images[0].rows()), static_cast<long>(eigen_images[0].cols()),
         static_cast<long>(eigen_images.size())};
  std::unique_ptr<CCfits::FITS> pFits(new CCfits::FITS(fits_name, FLOAT_IMG, 3, naxes));
  long fpixel(1);
  std::vector<long> extAx = {eigen_images[0].rows(), eigen_images[0].cols()};

  std::valarray<double> image(naxes[0] * naxes[1] * eigen_images.size());
  for(int i = 0; i < eigen_images.size(); i++)
    std::copy(eigen_images[i].data(), eigen_images[i].data() + eigen_images[i].size(),
              &image[naxes[0] * naxes[1] * i]);
  pFits->pHDU().addKey("AUTHOR", "Purify", "");
  pFits->pHDU().addKey("BUNIT", pix_units, "");
  pFits->pHDU().addKey("NAXIS", 3, "");
  pFits->pHDU().addKey("NAXIS1", naxes[0], "");
  pFits->pHDU().addKey("NAXIS2", naxes[1], "");
  pFits->pHDU().addKey("NAXIS3", eigen_images.size(), "");
  pFits->pHDU().write(fpixel, naxes[0] * naxes[1] * eigen_images.size(), image);
}

std::vector<Image<t_complex>> read3d(const std::string &fits_name) {
  /*
     Reads in a cube from a fits file and returns the vector of images.
     fits_name:: name of fits file
     */

  fitsfile *fptr;
  int status = 0;
  PURIFY_LOW_LOG("Reading fits file {}", fits_name);
  if(fits_open_image(&fptr, fits_name.c_str(), READONLY, &status))
    throw std::runtime_error("Error opening image " + fits_name);
  const t_int naxis = read_key<int>(fptr, "NAXIS", &status);
  const t_int rows = read_key<int>(fptr, "NAXIS1", &status);
  const t_int cols = read_key<int>(fptr, "NAXIS2", &status);
  const t_int channels = read_key<int>(fptr, "NAXIS3", &status);
  PURIFY_LOW_LOG("Axes {}", naxis);
  std::vector<long> fpixel(naxis, 1);
  t_int pols = 1;
  if(naxis > 3) {
    pols = read_key<int>(fptr, "NAXIS4", &status);
  }
  PURIFY_LOW_LOG("Dimensions {}x{}x{}x{}", rows, cols, channels, pols);
  if(pols > 1)
    throw std::runtime_error("Too many polarisations when reading " + fits_name);
  std::vector<Image<t_complex>> eigen_images;
  t_real nulval = 0;
  t_int anynul = 0;
  Vector<t_real> image = Vector<t_real>::Zero(rows * cols * channels * pols);
  if(fits_read_pix(fptr, TDOUBLE, fpixel.data(), static_cast<long>(image.size()), &nulval,
                   image.data(), &anynul, &status))
    throw std::runtime_error("Error reading data from image " + fits_name);
  if(anynul)
    PURIFY_LOW_LOG("There are bad values when reading " + fits_name);
  for(int i = 0; i < channels; i++) {
    Vector<t_complex> eigen_image = Vector<t_complex>::Zero(rows * cols);
    eigen_image.real() = image.segment(i * rows * cols, rows * cols);
    eigen_images.push_back(Image<t_complex>::Map(eigen_image.data(), rows, cols));
  }
  if(fits_close_file(fptr, &status))
    throw std::runtime_error("Problem closing fits file: " + fits_name);
  return eigen_images;
}
} // namespace pfitsio
} // namespace purify
