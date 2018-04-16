#include "purify/uvfits.h"
#include "purify/config.h"
#include <fitsio.h>
#include <iostream>
#include <memory>
#include <string>
#include "purify/logging.h"

namespace purify {
namespace pfitsio {

utilities::vis_params
read_uvfits(const std::vector<std::string> &names, const bool flag, const stokes pol) {
  utilities::vis_params output = read_uvfits(names.at(0), flag, pol);
  if(names.size() == 1)
    return output;
  for(int i = 1; i < names.size(); i++)
    output = read_uvfits(names.at(i), output);
  return output;
}
utilities::vis_params read_uvfits(const std::string &vis_name2, const utilities::vis_params &uv1,
                                  const bool flag, const stokes pol) {
  utilities::vis_params uv;
  const bool w_term = not uv1.w.isZero(0);
  const auto uv2 = read_uvfits(vis_name2, flag, pol);
  if(std::abs(uv1.ra - uv2.ra) > 1e-6)
    throw std::runtime_error(vis_name2 + ": wrong RA in pointing.");
  if(std::abs(uv1.dec - uv2.dec) > 1e-6)
    throw std::runtime_error(vis_name2 + ": wrong DEC in pointing.");
  uv.ra = uv1.ra;
  uv.dec = uv1.dec;
  uv.u = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.v = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.w = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.vis = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.weights = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.u.segment(0, uv1.size()) = uv1.u;
  uv.v.segment(0, uv1.size()) = uv1.v;
  uv.w.segment(0, uv1.size()) = uv1.w;
  uv.vis.segment(0, uv1.size()) = uv1.vis;
  uv.weights.segment(0, uv1.size()) = uv1.weights;
  uv.u.segment(uv1.size(), uv2.size()) = uv2.u;
  uv.v.segment(uv1.size(), uv2.size()) = uv2.v;
  uv.w.segment(uv1.size(), uv2.size()) = uv2.w;
  uv.vis.segment(uv1.size(), uv2.size()) = uv2.vis;
  uv.weights.segment(uv1.size(), uv2.size()) = uv2.weights;
  return uv;
}
utilities::vis_params read_uvfits(const std::string &filename, const bool flag, const stokes pol) {
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
    PURIFY_LOW_LOG("NAXIS {} Size: {}", i, n);
    total *= n;
  }
  const t_uint channels = naxis.at(3);
  const t_uint pols = naxis.at(2);
  const t_uint ifs = naxis.at(4);
  const t_uint pointings_num = naxis.at(5);
  PURIFY_MEDIUM_LOG("Pointings: {}", pointings_num);
  PURIFY_MEDIUM_LOG("IFs: {}", ifs);
  PURIFY_MEDIUM_LOG("Baselines: {}", baselines);
  PURIFY_MEDIUM_LOG("Channels: {}", channels);
  PURIFY_MEDIUM_LOG("Polarisations: {}", pols);
  PURIFY_MEDIUM_LOG("Total data per baseline: {}", total);
  if(pointings_num > 1)
    throw std::runtime_error("More than one pointing is not supported.");
  if(ifs > 1)
    throw std::runtime_error("More than one IF is not supported.");
  PURIFY_LOW_LOG("Reading Data...");

  const Matrix<t_real> coords = read_uvfits_coords(fptr, &status, baselines, pcount);

  //  (real, imag, weight, pol, frequency) is the order
  const Vector<t_real> data = read_uvfits_data(fptr, &status, naxis, baselines);
  const Vector<t_real> frequencies = read_uvfits_freq(fptr, &status, 4);
  if(frequencies.size() != channels)
    throw std::runtime_error("Number of frequencies doesn't match number of channels. "
                             + std::to_string(frequencies.size())
                             + "!=" + std::to_string(channels));
  t_int pol_index1;
  t_int pol_index2;
  Vector<t_complex> stokes_transform = Vector<t_complex>::Zero(2);
  switch(pol) {
  case stokes::I:
    pol_index1 = 0;
    pol_index2 = 1;
    stokes_transform(0) = 1. / 2;
    stokes_transform(1) = 1. / 2;
    break;
  case stokes::Q:
    pol_index1 = 0;
    pol_index2 = 1;
    stokes_transform(0) = 1. / 2;
    stokes_transform(1) = -1. / 2;
    break;
  case stokes::U:
    pol_index1 = 0;
    pol_index2 = 1;
    stokes_transform(0) = 1. / 2 * t_complex(0, -1);
    stokes_transform(1) = 1. / 2 * t_complex(0, -1);
    break;
  case stokes::V:
    pol_index1 = 0;
    pol_index2 = 1;
    stokes_transform(0) = 1. / 2 * t_complex(0, -1);
    stokes_transform(1) = -1. / 2 * t_complex(0, -1);
    break;
  default:
    pol_index1 = 0;
    pol_index2 = 1;
    stokes_transform(0) = 1. / 2;
    stokes_transform(1) = 1. / 2;
    throw std::runtime_error("Polarisation not supported for reading uvfits.");
    break;
  }
  auto const uv_data1
      = read_polarisation(data, coords, frequencies, pol_index1, pols, baselines, channels);
  auto const uv_data2
      = read_polarisation(data, coords, frequencies, pol_index2, pols, baselines, channels);
  PURIFY_MEDIUM_LOG("All Data Read!");
  fits_close_file(fptr, &status);
  if(status) { /* print any error messages */
    fits_report_error(stderr, status);
    throw std::runtime_error("Error reading fits file...");
  }
  return (flag) ? filter_and_combine(uv_data1, uv_data2, stokes_transform) :
                  filter_and_combine(uv_data1, uv_data2, stokes_transform,
                                     [](t_real, t_real, t_real, t_complex, t_complex, t_real,
                                        t_real, t_real, t_complex, t_complex) { return true; });
}
utilities::vis_params
filter_and_combine(const utilities::vis_params &input, const utilities::vis_params &input2,
                   const Vector<t_complex> stokes_transform,
                   const std::function<bool(t_real, t_real, t_real, t_complex, t_complex, t_real,
                                            t_real, t_real, t_complex, t_complex)> &filter) {
  t_uint size = 0;
  if(stokes_transform.size() != 2)
    throw std::runtime_error("stokes transform is not the right size (!= 2).");
  if(stokes_transform.isZero())
    throw std::runtime_error("stokes transform not valid.");
  if(input.size() != input2.size())
    throw std::runtime_error("Cannot apply filter to each data set, sizes do not match.");
  for(int i = 0; i < input.size(); i++) {
    if(filter(input.u(i), input.v(i), input.w(i), input.vis(i), input.weights(i), input2.u(i),
              input2.v(i), input2.w(i), input2.vis(i), input2.weights(i))) {
      if(std::abs(input.u(i) - input2.u(i)) > 1e-6)
        throw std::runtime_error("baselines don't match for filter.");
      if(std::abs(input.v(i) - input2.v(i)) > 1e-6)
        throw std::runtime_error("baselines don't match for filter.");
      if(std::abs(input.w(i) - input2.w(i)) > 1e-6)
        throw std::runtime_error("baselines don't match for filter.");
      size++;
    }
  }

  PURIFY_LOW_LOG("Applying flags: Keeping {} out of {} data points.", size, input.size());
  utilities::vis_params output(Vector<t_real>::Zero(size), Vector<t_real>::Zero(size),
                               Vector<t_real>::Zero(size), Vector<t_complex>::Zero(size),
                               Vector<t_complex>::Zero(size), input.units, input.ra, input.dec,
                               input.average_frequency);
  output.time = Vector<t_real>::Zero(size);
  output.baseline = Vector<t_uint>::Zero(size);
  output.frequencies = input.frequencies;
  t_uint count = 0;
  for(t_uint i = 0; i < input.size(); i++)
    if(filter(input.u(i), input.v(i), input.w(i), input.vis(i), input.weights(i), input2.u(i),
              input2.v(i), input2.w(i), input2.vis(i), input2.weights(i))) {
      output.u(count) = input.u(i);
      output.v(count) = input.v(i);
      output.w(count) = input.w(i);
      output.vis(count) = input.vis(i) * stokes_transform(0) + input2.vis(i) * stokes_transform(1);
      output.weights(count) = 1.
                              / std::sqrt(1. / input.weights(i) * stokes_transform(0)
                                          + 1. / input2.weights(i) * stokes_transform(1));
      output.baseline(count) = input.baseline(i);
      output.time(count) = input.time(i);
      count++;
    }

  return output;
}
utilities::vis_params read_polarisation(const Vector<t_real> &data, const Matrix<t_real> &coords,
                                        const Vector<t_real> &frequencies, const t_uint pol_index1,
                                        const t_uint pols, const t_uint baselines,
                                        const t_uint channels) {

  const t_uint stride = channels * baselines;
  utilities::vis_params uv_data;
  uv_data.u = Vector<t_real>::Zero(stride);
  uv_data.v = Vector<t_real>::Zero(stride);
  uv_data.w = Vector<t_real>::Zero(stride);
  uv_data.time = Vector<t_real>::Zero(stride);
  uv_data.baseline = Vector<t_uint>::Zero(stride);
  uv_data.vis = Vector<t_complex>::Zero(stride);
  uv_data.weights = Vector<t_complex>::Zero(stride);
  if(pol_index1 >= pols)
    throw std::runtime_error("Polarisation index out of bounds.");
  // reading in data
  t_uint count = 0;
  for(t_uint c = 0; c < channels; c++)
    for(t_uint b = 0; b < baselines; b++) {
      uv_data.vis(count) = read_vis_from_data(data, pol_index1, pols, c, channels, b, baselines);
      uv_data.weights(count)
          = read_weight_from_data(data, pol_index1, pols, c, channels, b, baselines);
      count++;
    }
  uv_data.average_frequency = frequencies.array().mean();
  for(t_uint i = 0; i < frequencies.size(); i++) {
    uv_data.u.segment(i * baselines, baselines) = coords.row(0) * frequencies(i);
    uv_data.v.segment(i * baselines, baselines) = -coords.row(1) * frequencies(i);
    uv_data.w.segment(i * baselines, baselines) = coords.row(2) * frequencies(i);
    uv_data.baseline.segment(i * baselines, baselines) = coords.row(3).cast<t_uint>();
    uv_data.time.segment(i * baselines, baselines) = coords.row(4);
  }
  uv_data.frequencies = frequencies;
  uv_data.units = utilities::vis_units::lambda;
  return uv_data;
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
    output(i) = i * dfreq + cfreq;
}
Vector<t_real> read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                                const t_int &baselines) {
  Vector<t_real> output;
  read_uvfits_data(fptr, status, naxis, baselines, output);
  return output;
}
void read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                      const t_int &baselines, Vector<t_real> &output) {
  long nelements = 1;
  for(int i = 2; i < naxis.size(); i++) {
    nelements *= static_cast<long>(naxis.at(i));
  }
  if(nelements == 0)
    throw std::runtime_error("Zero number of elements.");
  output = Vector<t_real>::Zero(naxis.at(1) * nelements * baselines);
  t_int nulval = static_cast<t_int>(NAN);
  t_int anynul = 0;
  fits_read_col(fptr, TDOUBLE, 2, 1, 1, static_cast<long>(output.size()), &nulval, output.data(),
                &anynul, status);
}
Matrix<t_real>
read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &groups, const t_int &pcount) {
  Matrix<t_real> output;
  read_uvfits_coords(fptr, status, pcount, groups, output);
  return output;
}
t_real read_value_from_data(const Vector<t_real> &data, const t_uint col, const t_uint pol,
                            const t_uint pols, const t_uint chan, const t_uint chans,
                            const t_uint baseline, const t_uint baselines) {
  const t_uint index = col + 3 * (pol + pols * (chan + chans * baseline));
  return data(index);
}
t_complex read_vis_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                             const t_uint chan, const t_uint chans, const t_uint baseline,
                             const t_uint baselines) {
  return t_complex(read_value_from_data(data, 0, pol, pols, chan, chans, baseline, baselines),
                   read_value_from_data(data, 1, pol, pols, chan, chans, baseline, baselines));
}
t_complex read_weight_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                                const t_uint chan, const t_uint chans, const t_uint baseline,
                                const t_uint baselines) {
  return t_complex(read_value_from_data(data, 2, pol, pols, chan, chans, baseline, baselines), 0);
}
void read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &pcount, const t_int &groups,
                        Matrix<t_real> &output) {
  output = Matrix<t_real>::Zero(pcount, groups);
  t_int nulval = static_cast<t_int>(NAN);
  t_int anynul;
  // reading in parameters per baseline
  for(int i = 0; i < groups; i++)
    fits_read_col(fptr, TDOUBLE, 1, 1 + i, 1, pcount, &nulval, output.col(i).data(), &anynul,
                  status);
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
