#ifndef PURIFY_PFITSIO_H
#define PURIFY_PFITSIO_H
#include "purify/config.h"
#include "purify/logging.h"
#include "purify/types.h"

#include <fitsio.h>
#include <string>
#include "purify/utilities.h"

namespace purify {

namespace pfitsio {
struct header_params {
  // structure containing parameters for fits header
  std::string fits_name = "output_image.fits";
  t_real mean_frequency = 0; // in MHz
  t_real cell_x = 1;            // in arcseconds
  t_real cell_y = 1;            // in arcseconds
  t_real ra = 0;                // in radians, converted to decimal degrees before write
  t_real dec = 0;               // in radians, converted to decimal degrees before write
  t_int pix_ref_x = 0;
  t_int pix_ref_y = 0;
  std::string pix_units = "Jy/BEAM";
  t_real channels_total = 1;
  t_real channel_width = 8; // in MHz
  t_real polarisation = 1;
  t_int niters = 0;          // number of iterations
  bool hasconverged = false; // stating if model has converged
  t_real relative_variation = 0;
  t_real residual_convergence = 0;
  t_real epsilon = 0;
  //! create fits header object and fill
  header_params(const std::string &fits_name_, const std::string & pix_units_,
      const t_real channels_total_,
      const t_real ra_, const t_real dec_, 
      const stokes pol, 
      const t_real cellx_, const t_real celly_,const t_real mean_frequency_,  
      const t_real channel_width_,
      const t_uint niters_, const bool hasconverged_,
      const t_real relative_variation_, const t_real residual_convergence_,
      const t_real epsilon_) 
    : fits_name(fits_name_), pix_units(pix_units_),
      channels_total(channels_total_),
      mean_frequency(mean_frequency_), channel_width(channel_width_),
      cell_x(cellx_), cell_y(celly_), ra(ra_), dec(dec_),
      niters(niters_),
      hasconverged(hasconverged_), epsilon(epsilon_),
      relative_variation(relative_variation_),
      residual_convergence(residual_convergence_)
       {
         try
         {
             this->polarisation = stokes_int.at(pol);
          } 
          catch (std::out_of_range & e)
          {
                 PURIFY_LOW_LOG("Polarisation type not recognised by FITS, assuming Stokes I.");
                 this->polarisation = stokes_int.at(stokes::I);
          };
       };
  //! create empty fits header
  header_params() {};
#define  PURIFY_MACRO(VAR, H2) \
    (this->VAR == H2.VAR)
  bool operator==(const header_params &h2) const{
  return
    PURIFY_MACRO(mean_frequency, h2) and
    PURIFY_MACRO(cell_x, h2) and
    PURIFY_MACRO(cell_y, h2) and
    PURIFY_MACRO(ra, h2) and
    PURIFY_MACRO(dec, h2) and
    PURIFY_MACRO(pix_ref_x, h2) and
    PURIFY_MACRO(pix_ref_y, h2) and
    PURIFY_MACRO(pix_units, h2) and
    PURIFY_MACRO(channels_total, h2) and
    PURIFY_MACRO(channel_width, h2) and
    PURIFY_MACRO(polarisation, h2) and
    PURIFY_MACRO(niters, h2) and
    PURIFY_MACRO(hasconverged, h2) and
    PURIFY_MACRO(relative_variation, h2) and
    PURIFY_MACRO(residual_convergence, h2) and
    PURIFY_MACRO(epsilon, h2);
  }
#undef PURIFY_MACRO
  bool operator!=(const header_params &h2) const{
    return not (*this == h2);
  }
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
typename std::enable_if<std::is_scalar<T>::value, void>::type
write_history(fitsfile *fptr, const std::string &context, const T &history, int *status) {
  T value = history;
  write_history(fptr, context, std::to_string(value), status);
}
template <class T>
typename std::enable_if<std::is_scalar<T>::value, void>::type
write_key(fitsfile *fptr, const std::string &key, const T &value, const std::string &comment,
          int *status) {
  int datatype = 0;
  if(std::is_same<T, double>::value)
    datatype = TDOUBLE;
  else if(std::is_same<T, float>::value)
    datatype = TFLOAT;
  else if(std::is_same<T, int>::value)
    datatype = TINT;
  else if(std::is_same<T, bool>::value)
    datatype = TLOGICAL;
  else
    throw std::runtime_error("Key type of value not supported by PURIFY template for " + key);

  T entry = value;
  if(fits_write_key(fptr, datatype, const_cast<char *>(key.c_str()),
                    reinterpret_cast<void *>(&entry), const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Problem writing key in fits file: " + key);
}

//! read fits key and return as tuple
template <class T>
typename std::enable_if<std::is_scalar<T>::value, T>::type
read_key(fitsfile *fptr, const std::string &key, int *status) {

  int datatype = 0;
  if(std::is_same<T, double>::value)
    datatype = TDOUBLE;
  else if(std::is_same<T, float>::value)
    datatype = TFLOAT;
  else if(std::is_same<T, int>::value)
    datatype = TINT;
  else if(std::is_same<T, bool>::value)
    datatype = TLOGICAL;
  else
    throw std::runtime_error("Key type of value not supported by PURIFY template for " + key);
  T value;
  std::string comment = "";
  if(fits_read_key(fptr, datatype, const_cast<char *>(key.c_str()), &value,
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
                    const pfitsio::header_params &header, const bool &overwrite = true) {
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
  for(int i = 0; i < input.size(); i++) {
    Image<t_real> const data = input.derived().real().template cast<t_real>();
    images.push_back(Image<t_real>::Map(data.data(), nx, ny));
  }
  write3d(images, fits_name, pix_units, overwrite);
}
template <class DERIVED>
void write3d(const std::vector<Eigen::EigenBase<DERIVED>> &input, int nx, int ny,
                    const pfitsio::header_params &header, const bool &overwrite = true) {
  std::vector<Image<t_real>> images;
  for(int i = 0; i < input.size(); i++) {
    Image<t_real> const data = input.derived().real().template cast<t_real>();
    images.push_back(Image<t_real>::Map(data.data(), nx, ny));
  }
  write3d(images, header, overwrite);
}
//! Read cube from fits file
std::vector<Image<t_complex>> read3d(const std::string &fits_name);
} // namespace pfitsio
} // namespace purify

#endif
