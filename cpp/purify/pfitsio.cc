#include "purify/config.h"
#include "purify/logging.h"

#include "purify/pfitsio.h"
namespace purify {
namespace pfitsio {
void write_key(fitsfile *fptr, const std::string &key, const std::string &value,
               const std::string &comment, int *status) {
  std::string entry = value;
  if (fits_write_key(fptr, TSTRING, const_cast<char *>(key.c_str()),
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
  if (fits_write_history(fptr, const_cast<char *>(entry.c_str()), status))
    throw std::runtime_error("Problem writing comments in fits file");
}
std::string read_key(fitsfile *fptr, const std::string &key, int *status) {
  std::string value = "";
  std::string comment = "";
  if (fits_read_key(fptr, TSTRING, const_cast<char *>(key.c_str()),
                    const_cast<char *>(value.c_str()), const_cast<char *>(comment.c_str()), status))
    throw std::runtime_error("Error reading value from key " + key);
  return value;
}
//! Write image to fits file
void write2d(const Image<t_real> &eigen_image, const pfitsio::header_params &header,
             const bool &overwrite) {
  /*
    Writes an image to a fits file.

    image:: image data, a 2d Image.
        header:: structure containing header information
        overwrite:: if true, overwrites old fits file with same name

  */

  write3d(std::vector<Image<t_real>>(1, eigen_image), header, overwrite);
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
  pfitsio::header_params header;
  header.fits_name = fits_name;
  header.pix_units = pix_units;

  write2d(eigen_image, header, overwrite);
}

Image<t_complex> read2d(const std::string &fits_name) {
  /*
    Reads in an image from a fits file and returns the image.

    fits_name:: name of fits file
  */

  std::vector<Image<t_complex>> images = read3d(fits_name);
  return images.at(0);
}

void write3d(const std::vector<Image<t_real>> &eigen_images, const pfitsio::header_params &header,
             const bool &overwrite) {
  std::vector<long> naxes = {static_cast<long>(eigen_images.at(0).rows()),
                             static_cast<long>(eigen_images.at(0).cols()),
                             static_cast<long>(eigen_images.size()), 1};
  std::vector<long> fpixel = {1, 1, 1, 1};

  Vector<t_real> image = Vector<t_real>::Zero(naxes.at(0) * naxes.at(1) * naxes.at(2));
  for (int i = 0; i < eigen_images.size(); i++)
    image.segment(i * naxes.at(0) * naxes.at(1), naxes.at(0) * naxes.at(1)) =
        Vector<t_real>::Map(eigen_images.at(i).data(), naxes.at(0) * naxes.at(1));
  write3d<Vector<t_real>>(image, naxes.at(0), naxes.at(1), naxes.at(2), header, overwrite);
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
  pfitsio::header_params header;
  header.fits_name = fits_name;
  header.pix_units = pix_units;

  write3d(eigen_images, header, overwrite);
}

std::vector<Image<t_complex>> read3d(const std::string &fits_name) {
  std::vector<Image<t_complex>> eigen_images;
  Vector<t_real> image;
  t_int rows, cols, channels, pols = 1;
  read3d<Vector<t_real>>(fits_name, image, rows, cols, channels, pols);
  for (int i = 0; i < channels; i++) {
    Vector<t_complex> eigen_image = Vector<t_complex>::Zero(rows * cols);
    eigen_image.real() = image.segment(i * rows * cols, rows * cols);
    eigen_images.push_back(Image<t_complex>::Map(eigen_image.data(), rows, cols));
  }
  return eigen_images;
}
}  // namespace pfitsio
}  // namespace purify
