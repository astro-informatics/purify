#include "purify/uvfits.h"
#include "purify/config.h"
#include <fitsio.h>
#include <iostream>
#include <memory>
#include <string>
#include "purify/logging.h"

namespace purify {
namespace pfitsio {

utilities::vis_params read_uvfits(const std::string &filename, const bool flag) {
  utilities::vis_params uv_data;
  fitsfile *fptr;
  t_int status = 0;
  t_int hdupos;
  t_int baselines;
  t_int naxes;
  t_int pcount;
  t_int stokes;
  std::shared_ptr<char> comment
      = std::shared_ptr<char>(new char[FLEN_CARD], [](char *ptr) { delete[] ptr; });
  PURIFY_MEDIUM_LOG("Reading uvfits {}", filename);
  if(fits_open_file(&fptr, filename.c_str(), READONLY, &status))
    throw std::runtime_error("Could not open file " + filename);

  t_int hdutype;
  if(fits_movabs_hdu(fptr, 1, &hdutype, &status))
    throw std::runtime_error("Error changing HDU.");
  if(hdutype != IMAGE_HDU)
    throw std::runtime_error("HDU 1 not expected type " + std::to_string(hdutype));

  fits_read_key(fptr, TDOUBLE, "CRVAL5", &uv_data.ra, comment.get(), &status);
  fits_read_key(fptr, TDOUBLE, "CRVAL6", &uv_data.dec, comment.get(), &status);
  // fits_read_key(fptr, TINT, "CRVAL3", &stokes, comment.get(), &status);
  fits_read_key(fptr, TINT, "GCOUNT", &baselines, comment.get(), &status);
  fits_read_key(fptr, TINT, "NAXIS", &naxes, comment.get(), &status);
  fits_read_key(fptr, TINT, "PCOUNT", &pcount, comment.get(), &status);
  if(naxes == 0)
    throw std::runtime_error("No axes in header... ");
  if(pcount == 0)
    throw std::runtime_error("No uvw or time coordinates in header... ");
  t_uint total = 1;
  std::vector<t_int> naxis(naxes, 0);
  // filling up axis sizes
  for(int i = 1; i < naxes; i++) {
    t_int n;
    std::string key = "NAXIS" + std::to_string(i + 1);
    fits_read_key(fptr, TINT, key.c_str(), &n, comment.get(), &status);
    naxis[i] = n;
    total *= n;
  }
  const t_uint channels = naxis.at(3);
  const t_uint pols = naxis.at(2);
  PURIFY_MEDIUM_LOG("Baselines: {}", baselines);
  PURIFY_MEDIUM_LOG("Channels: {}", channels);
  PURIFY_MEDIUM_LOG("Polarisations: {}", pols);
  PURIFY_MEDIUM_LOG("Total data per baseline: {}", total);
  PURIFY_LOW_LOG("Reading Data...");
  const Matrix<t_real> coords = read_uvfits_coords(fptr, &status, baselines, pcount);

  const Matrix<t_real> data = read_uvfits_data(fptr, &status, naxis, pcount, baselines);
  const t_uint stride = channels * baselines;
  uv_data.u = Vector<t_real>::Zero(stride);
  uv_data.v = Vector<t_real>::Zero(stride);
  uv_data.w = Vector<t_real>::Zero(stride);
  uv_data.vis = Vector<t_complex>::Zero(stride);
  uv_data.weights = Vector<t_complex>::Zero(stride);
  const Matrix<t_real> I
      = (data.block(0, 2 * stride, 3, stride) + data.block(0, 3 * stride, 3, stride))
        / 2; // assuming xy, yx, xx, yy is the order
  uv_data.vis.real() = I.row(0);
  uv_data.vis.imag() = I.row(1);
  uv_data.weights.real() = I.row(2);

  const Vector<t_real> frequencies = read_uvfits_freq(fptr, &status, 4);
  uv_data.average_frequency = frequencies.array().mean();
  if(frequencies.size() != channels)
    throw std::runtime_error("Number of frequencies doesn't match number of channels. "
                             + std::to_string(frequencies.size())
                             + "!=" + std::to_string(channels));
  for(t_uint i = 0; i < frequencies.size(); i++) {
    uv_data.u.segment(i * baselines, baselines) = coords.row(0) * frequencies(i);
    uv_data.v.segment(i * baselines, baselines) = coords.row(1) * frequencies(i);
    uv_data.w.segment(i * baselines, baselines) = coords.row(2) * frequencies(i);
  }
  uv_data.units = utilities::vis_units::lambda;
  PURIFY_LOW_LOG("All Data Read!");
  fits_close_file(fptr, &status);
  if(status) { /* print any error messages */
    fits_report_error(stderr, status);
    throw std::runtime_error("Error reading fits file...");
  }
  return (flag) ? filter(uv_data) : uv_data;
}
utilities::vis_params filter(const utilities::vis_params &input) {
  t_uint size = 0;
  for(int i = 0; i < input.size(); i++) {
    if((std::abs(input.weights(i)) > 0)
       and (!std::isnan(input.vis(i).real()) and !std::isnan(input.vis(i).imag())))
      size++;
  }

  PURIFY_LOW_LOG("Applying flags: Keeping {} out of {} data points.", size, input.size());
  utilities::vis_params output(Vector<t_real>::Zero(size), Vector<t_real>::Zero(size),
                               Vector<t_real>::Zero(size), Vector<t_complex>::Zero(size),
                               Vector<t_complex>::Zero(size), input.units, input.ra, input.dec,
                               input.average_frequency);
  t_uint count = 0;
  for(int i = 0; i < input.size(); i++) {
    if((std::abs(input.weights(i)) > 0)
       and (!std::isnan(input.vis(i).real()) and !std::isnan(input.vis(i).imag()))) {
      output.u(count) = input.u(i);
      output.v(count) = input.w(i);
      output.w(count) = input.v(i);
      output.weights(count) = input.weights(i);
      output.vis(count) = input.vis(i);
      count++;
    }
  }
  return output;
}
Vector<t_real> read_uvfits_freq(fitsfile *fptr, t_int *status, const t_int &col) {
  Vector<t_real> output;
  read_uvfits_freq(fptr, status, output, col);
  return output;
}
void read_uvfits_freq(fitsfile *fptr, t_int *status, Vector<t_real> &output, const t_int &col) {
  t_real cfreq;
  t_real dfreq;
  t_int crpix;
  t_int nfreq;
  std::shared_ptr<char> comment
      = std::shared_ptr<char>(new char[FLEN_CARD], [](char *ptr) { delete[] ptr; });
  std::string key = "CRVAL" + std::to_string(col);
  fits_read_key(fptr, TDOUBLE, key.c_str(), &cfreq, comment.get(), status);
  key = "CDELT" + std::to_string(col);
  fits_read_key(fptr, TDOUBLE, key.c_str(), &dfreq, comment.get(), status);
  key = "CRPIX" + std::to_string(col);
  fits_read_key(fptr, TINT, key.c_str(), &crpix, comment.get(), status);
  key = "NAXIS" + std::to_string(col);
  fits_read_key(fptr, TINT, key.c_str(), &nfreq, comment.get(), status);
  if(nfreq == 0)
    throw std::runtime_error("Wrong number of channels read from header.");
  output = Vector<t_real>::Zero(nfreq);
  for(t_int i = 0; i < output.size(); i++)
    output(i) = (i + 1 - crpix) * dfreq + cfreq;
}
Matrix<t_real> read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                                const t_int &pcount, const t_int &baselines) {
  Matrix<t_real> output;
  read_uvfits_data(fptr, status, naxis, pcount, baselines, output);
  return output;
}
void read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                      const t_int &pcount, const t_int &baselines, Matrix<t_real> &output) {
  long nelements = 1;
  for(int i = 2; i < naxis.size(); i++) {
    nelements *= static_cast<long>(naxis.at(i));
  }
  if(nelements == 0)
    throw std::runtime_error("Zero number of elements.");
  output = Matrix<t_real>::Zero(naxis.at(1), nelements * baselines);
  t_int nulval = 0;
  t_int anynul;
  fits_read_col(fptr, TDOUBLE, 2, 1, 1, output.size(), &nulval, output.data(), &anynul, status);
}
Matrix<t_real>
read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &groups, const t_int &pcount) {
  Matrix<t_real> output;
  read_uvfits_coords(fptr, status, groups, pcount, output);
  return output;
}
void read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &pcount, const t_int &groups,
                        Matrix<t_real> &output) {
  output = Matrix<t_real>::Zero(groups, pcount);
  t_int nulval = 0;
  t_int anynul;
  fits_read_col(fptr, TDOUBLE, 1, 1, 1, output.size(), &nulval, output.data(), &anynul, status);
}
void read_fits_keys(fitsfile *fptr, t_int *status) {

  std::shared_ptr<char> card
      = std::shared_ptr<char>(new char[FLEN_CARD], [](char *ptr) { delete[] ptr; });
  t_int nkeys;
  fits_get_hdrspace(fptr, &nkeys, NULL, status);

  for(t_int ii = 1; ii <= nkeys; ii++) {
    fits_read_record(fptr, ii, card.get(), status); /* read keyword */
    std::printf("%s\n", card.get());
  }
  std::printf("END\n\n"); /* terminate listing with END */
}

} // namespace pfitsio
} // namespace purify
