
#ifndef PURIFY_PUVFITS_H
#define PURIFY_PUVFITS_H
#include "purify/config.h"
#include "purify/logging.h"
#include "purify/types.h"

#include <fitsio.h>
#include <string>
#include "purify/utilities.h"

namespace purify {

namespace pfitsio {

//! Read uvfits file
utilities::vis_params read_uvfits(const std::string &filename, const bool flag = true);
//! Remove visibilities with zero weighting
utilities::vis_params filter(const utilities::vis_params &input);
//! Read uvfits keys out
void read_fits_keys(fitsfile *fptr, t_int *status);
//! read frequencies for each channel
Vector<t_real> read_uvfits_freq(fitsfile *fptr, t_int *status, const t_int &col);
void read_uvfits_freq(fitsfile *fptr, t_int *status, Vector<t_real> &output, const t_int &col);
//! read coordinates from uvfits file
Matrix<t_real>
read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &groups, const t_int &pcount);
void read_uvfits_coords(fitsfile *fptr, t_int *status, const t_int &groups, const t_int &pcount,
                        Matrix<t_real> &output);
//! read data from uvfits file
Matrix<t_real> read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                                const t_int &pcount, const t_int &baselines);
void read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                      const t_int &pcount, const t_int &baselines, Matrix<t_real> &output);
} // namespace pfitsio
} // namespace purify

#endif
