
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
enum class stokes { I, Q, U, V, XX, YY, XY, YX };

//! Read uvfits file
utilities::vis_params
read_uvfits(const std::string &filename, const bool flag = true, const stokes pol = stokes::I);
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
Vector<t_real> read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                                const t_int &baselines);
void read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                      const t_int &baselines, Vector<t_real> &output);
//! read value from data
t_real read_value_from_data(const Vector<t_real> &data, const t_uint col, const t_uint pol,
                            const t_uint pols, const t_uint chan, const t_uint chans,
                            const t_uint baseline);
//! return visibility for given baseline
t_complex read_vis_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                             const t_uint chan, const t_uint chans, const t_uint baseline);
//! return weight for given baseline
t_complex read_weight_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                                const t_uint chan, const t_uint chans, const t_uint baseline);
} // namespace pfitsio
} // namespace purify

#endif
