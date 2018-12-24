#ifndef PURIFY_PFITSIO_H
#define PURIFY_PFITSIO_H
#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"

#include <fitsio.h>
#include <string>
#include "purify/utilities.h"

namespace purify {

namespace pfitsio {
struct header_params {
  // structure containing parameters for fits header
  std::string fits_name = "output_image.fits";
  t_real mean_frequency = 0;  // in MHz
  t_real cell_x = 1;          // in arcseconds
  t_real cell_y = 1;          // in arcseconds
  t_real ra = 0;              // in radians, converted to decimal degrees before write
  t_real dec = 0;             // in radians, converted to decimal degrees before write
  t_int pix_ref_x = 0;
  t_int pix_ref_y = 0;
  std::string pix_units = "Jy/BEAM";
  t_real channels_total = 1;
  t_real channel_width = 8;  // in MHz
  t_real polarisation = 1;
  t_int niters = 0;           // number of iterations
  bool hasconverged = false;  // stating if model has converged
  t_real relative_variation = 0;
  t_real residual_convergence = 0;
  t_real epsilon = 0;
  //! create fits header object and fill
  header_params(const std::string &fits_name_, const std::string &pix_units_,
                const t_real channels_total_, const t_real ra_, const t_real dec_, const stokes pol,
                const t_real cellx_, const t_real celly_, const t_real mean_frequency_,
                const t_real channel_width_, const t_uint niters_, const bool hasconverged_,
                const t_real relative_variation_, const t_real residual_convergence_,
                const t_real epsilon_)
      : fits_name(fits_name_),
        pix_units(pix_units_),
        channels_total(channels_total_),
        mean_frequency(mean_frequency_),
        channel_width(channel_width_),
        cell_x(cellx_),
        cell_y(celly_),
        ra(ra_),
        dec(dec_),
        niters(niters_),
        hasconverged(hasconverged_),
        epsilon(epsilon_),
        relative_variation(relative_variation_),
        residual_convergence(residual_convergence_) {
    try {
      this->polarisation = stokes_int.at(pol);
    } catch (std::out_of_range &e) {
      PURIFY_LOW_LOG("Polarisation type not recognised by FITS, assuming Stokes I.");
      this->polarisation = stokes_int.at(stokes::I);
    };
  };
  //! create empty fits header
  header_params(){};
#define PURIFY_MACRO(VAR, H2) (this->VAR == H2.VAR)
  bool operator==(const header_params &h2) const {
    return PURIFY_MACRO(mean_frequency, h2) and PURIFY_MACRO(cell_x, h2) and
           PURIFY_MACRO(cell_y, h2) and PURIFY_MACRO(ra, h2) and PURIFY_MACRO(dec, h2) and
           PURIFY_MACRO(pix_ref_x, h2) and PURIFY_MACRO(pix_ref_y, h2) and
           PURIFY_MACRO(pix_units, h2) and PURIFY_MACRO(channels_total, h2) and
           PURIFY_MACRO(channel_width, h2) and PURIFY_MACRO(polarisation, h2) and
           PURIFY_MACRO(niters, h2) and PURIFY_MACRO(hasconverged, h2) and
           PURIFY_MACRO(relative_variation, h2) and PURIFY_MACRO(residual_convergence, h2) and
           PURIFY_MACRO(epsilon, h2);
  }
#undef PURIFY_MACRO
  bool operator!=(const header_params &h2) const { return not(*this == h2); }
};
//! write key to fits file header
void write_key(fitsfile *fptr, const std::string &key, const std::string &value,
               const std::string &comment, int *status);
void write_key(fitsfile *fptr, const std::string &key, const char *value,
               const std::string &comment, int *status);
//! write history to fits file
void write_history(fitsfile *fptr, const std::string &context, const std::string &history,
                   int *status);
template <class T>
typename std::enable_if<std::is_scalar<T>::value, void>::type write_history(
    fitsfile *fptr, const std::string &context, const T &history, int *status) {
  T value = history;
  write_history(fptr, context, std::to_string(value), status);
}
template <class T>
typename std::enable_if<std::is_scalar<T>::value, void>::type write_key(fitsfile *fptr,
                                                                        const std::string &key,
                                                                        const T &value,
                                                                        const std::string &comment,
                                                                        int *status) {
  int datatype = 0;
  if (std::is_same<T, double>::value)
    datatype = TDOUBLE;
  else if (std::is_same<T, float>::value)
    datatype = TFLOAT;
  else if (std::is_same<T, int>::value)
    datatype = TINT;
  else if (std::is_same<T, bool>::value)
    datatype = TLOGICAL;
  else
    throw std::runtime_error("Key type of value not supported by PURIFY template for " + key);

  T entry = value;
  if (fits_write_key(fptr, datatype, const_cast<char *>(key.c_str()),
                     reinterpret_cast<void *>(&entry), const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Problem writing key in fits file: " + key);
}

//! read fits key and return as tuple
template <class T>
typename std::enable_if<std::is_scalar<T>::value, T>::type read_key(fitsfile *fptr,
                                                                    const std::string &key,
                                                                    int *status) {
  int datatype = 0;
  if (std::is_same<T, double>::value)
    datatype = TDOUBLE;
  else if (std::is_same<T, float>::value)
    datatype = TFLOAT;
  else if (std::is_same<T, int>::value)
    datatype = TINT;
  else if (std::is_same<T, bool>::value)
    datatype = TLOGICAL;
  else
    throw std::runtime_error("Key type of value not supported by PURIFY template for " + key);
  T value;
  std::string comment = "";
  if (fits_read_key(fptr, datatype, const_cast<char *>(key.c_str()), &value,
                    const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Error reading value from key " + key);
  return value;
}
std::string read_key(fitsfile *fptr, const std::string &key, int *status);
//! Write image to fits file using header information
void write2d(const Image<t_real> &image, const pfitsio::header_params &header,
             const bool &overwrite = true);
//! Write image to fits file
void write2d(const Image<t_real> &image, const std::string &fits_name,
             const std::string &pix_units = "Jy/Beam", const bool &overwrite = true);

template <class DERIVED>
void write2d(const Eigen::EigenBase<DERIVED> &input, int nx, int ny, const std::string &fits_name,
             const std::string &pix_units = "Jy/Beam", const bool &overwrite = true) {
  Image<t_real> const data = input.derived().real().template cast<t_real>();
  write2d(Image<t_real>::Map(data.data(), nx, ny), fits_name, pix_units, overwrite);
}
template <class DERIVED>
void write2d(const Eigen::EigenBase<DERIVED> &input, int nx, int ny,
             const pfitsio::header_params &header, const bool overwrite = true) {
  Image<t_real> const data = input.derived().real().template cast<t_real>();
  write2d(Image<t_real>::Map(data.data(), nx, ny), header, overwrite);
}
//! Read image from fits file
Image<t_complex> read2d(const std::string &fits_name);
//! Write cube to fits file using header information
void write3d(const std::vector<Image<t_real>> &image, const pfitsio::header_params &header,
             const bool &overwrite = true);
//! Write cube to fits file
void write3d(const std::vector<Image<t_real>> &image, const std::string &fits_name,
             const std::string &pix_units = "Jy/Beam", const bool &overwrite = true);
template <class DERIVED>
void write3d(const std::vector<Eigen::EigenBase<DERIVED>> &input, int nx, int ny,
             const std::string &fits_name, const std::string &pix_units = "Jy/Beam",
             const bool &overwrite = true) {
  std::vector<Image<t_real>> images;
  for (int i = 0; i < input.size(); i++) {
    Image<t_real> const data = input.derived().real().template cast<t_real>();
    images.push_back(Image<t_real>::Map(data.data(), nx, ny));
  }
  write3d(images, fits_name, pix_units, overwrite);
}
template <class DERIVED>
void write3d(const std::vector<Eigen::EigenBase<DERIVED>> &input, int nx, int ny,
             const pfitsio::header_params &header, const bool &overwrite = true) {
  std::vector<Image<t_real>> images;
  for (int i = 0; i < input.size(); i++) {
    Image<t_real> const data = input.derived().real().template cast<t_real>();
    images.push_back(Image<t_real>::Map(data.data(), nx, ny));
  }
  write3d(images, header, overwrite);
}
template <class T>
typename std::enable_if<std::is_same<t_real, typename T::Scalar>::value, void>::type write3d(
    const Eigen::EigenBase<T> &image, const t_int rows, const t_int cols, const t_int chans,
    const std::string &fits_name, const std::string &pix_units = "Jy/Beam",
    const bool &overwrite = true) {
  pfitsio::header_params header;
  header.fits_name = fits_name;
  header.pix_units = pix_units;
  write3d<T>(image, rows, cols, chans, header, overwrite);
}
//! write 3d fits cube with header
template <class T>
typename std::enable_if<std::is_same<t_real, typename T::Scalar>::value, void>::type write3d(
    const Eigen::EigenBase<T> &image, const t_int rows, const t_int cols, const t_int chans,
    const pfitsio::header_params &header, const bool &overwrite) {
  /*
     Writes an image to a fits file.
     image:: image data, a 2d Image.
     header:: structure containing header information
     overwrite:: if true, overwrites old fits file with same name
*/
  if (image.size() != rows * cols * chans)
    throw std::runtime_error("image or cube size does not match dimensions.");

  PURIFY_LOW_LOG("Writing fits file {}", header.fits_name);
  if (overwrite == true) {
    remove(header.fits_name.c_str());
  }
  fitsfile *fptr;
  t_int status = 0;
  std::vector<long> naxes = {static_cast<long>(rows), static_cast<long>(cols),
                             static_cast<long>(chans), 1};
  std::vector<long> fpixel = {1, 1, 1, 1};

  if (fits_create_file(&fptr, header.fits_name.c_str(), &status))
    throw std::runtime_error("Problem creating fits file:" + header.fits_name);
  if (fits_create_img(fptr, DOUBLE_IMG, static_cast<t_int>(naxes.size()), naxes.data(), &status))
    throw std::runtime_error("Problem creating HDU for image in fits file:" + header.fits_name);
  if (fits_write_pix(fptr, TDOUBLE, fpixel.data(), static_cast<long>(image.size()),
                     const_cast<t_real *>(image.derived().data()), &status))
    throw std::runtime_error("Problem writing image in fits file:" + header.fits_name);

#define PURIFY_MACRO(KEY, VALUE, COMMENT) write_key(fptr, KEY, VALUE, COMMENT, &status);

  // Writing to fits header
  PURIFY_MACRO("BUNIT", header.pix_units, "");                  // units
  PURIFY_MACRO("NAXIS", static_cast<t_int>(naxes.size()), "");  // number of axes
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
  PURIFY_MACRO("CTYPE1", "RA--SIN", "");
  PURIFY_MACRO("CTYPE2", "DEC--SIN", "");
  PURIFY_MACRO("CTYPE3", "FREQ", "");
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
  if (header.hasconverged) {
    write_history(fptr, "PURIFY-CONVERGED", "True", &status);
  } else {
    write_history(fptr, "PURIFY-CONVERGED", "False", &status);
  }
  write_history(fptr, "PURIFY-RELATIVEVARIATION", header.relative_variation, &status);
  write_history(fptr, "PURIFY-RESIDUALCONVERGENCE", header.residual_convergence, &status);
  write_history(fptr, "PURIFY-EPSILON", header.epsilon, &status);
  if (fits_write_date(fptr, &status))
    throw std::runtime_error("Problem writing date in fits file:" + header.fits_name);

  if (fits_close_file(fptr, &status))
    throw std::runtime_error("Problem closing fits file:" + header.fits_name);
}

//! Read cube from fits file
std::vector<Image<t_complex>> read3d(const std::string &fits_name);
template <class T>
typename std::enable_if<std::is_same<t_real, typename T::Scalar>::value, void>::type read3d(
    const std::string &fits_name, Eigen::EigenBase<T> &output, t_int &rows, t_int &cols,
    t_int &channels, t_int &pols) {
  /*
     Reads in a cube from a fits file and returns the vector of images.
     fits_name:: name of fits file
     */

  fitsfile *fptr;
  int status = 0;
  PURIFY_LOW_LOG("Reading fits file {}", fits_name);
  if (fits_open_image(&fptr, fits_name.c_str(), READONLY, &status))
    throw std::runtime_error("Error opening image " + fits_name);
  const t_int naxis = read_key<int>(fptr, "NAXIS", &status);
  if (naxis < 1) throw std::runtime_error("Image contains zero axes.");
  rows = read_key<int>(fptr, "NAXIS1", &status);
  cols = (naxis > 1) ? read_key<int>(fptr, "NAXIS2", &status) : 1;
  channels = (naxis > 2) ? read_key<int>(fptr, "NAXIS3", &status) : 1;
  pols = (naxis > 3) ? read_key<int>(fptr, "NAXIS4", &status) : 1;
  PURIFY_LOW_LOG("Axes {}", naxis);
  std::vector<long> fpixel(naxis, 1);
  PURIFY_LOW_LOG("Dimensions {}x{}x{}x{}", rows, cols, channels, pols);
  if (pols > 1) throw std::runtime_error("Too many polarisations when reading " + fits_name);
  t_real nulval = 0;
  t_int anynul = 0;
  output.derived() = Vector<typename T::Scalar>::Zero(rows * cols * channels * pols);
  if (fits_read_pix(fptr, TDOUBLE, fpixel.data(), static_cast<long>(output.size()), &nulval,
                    output.derived().data(), &anynul, &status))
    throw std::runtime_error("Error reading data from image " + fits_name);
  if (anynul) PURIFY_LOW_LOG("There are bad values when reading " + fits_name);
  if (fits_close_file(fptr, &status))
    throw std::runtime_error("Problem closing fits file: " + fits_name);
}
}  // namespace pfitsio
}  // namespace purify

#endif
