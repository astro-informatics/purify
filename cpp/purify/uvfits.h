
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
//! Read uvfits files from name of vector
utilities::vis_params read_uvfits(const std::vector<std::string> &names, const bool flag = true,
                                  const stokes pol = stokes::I);
//! Reads in combine visiblities from uvfits files
utilities::vis_params read_uvfits(const std::string &vis_name2, const utilities::vis_params &u1,
                                  const bool flag = true, const stokes pol = stokes::I);
//! Remove visibilities with zero weighting
utilities::vis_params
filter_and_combine(const utilities::vis_params &input, const utilities::vis_params &input2,
                   const Vector<t_complex> stokes_transform,
                   const std::function<bool(t_real, t_real, t_real, t_complex, t_complex, t_real,
                                            t_real, t_real, t_complex, t_complex)> &filter
                   = [](const t_real, const t_real, const t_real, const t_complex vis1,
                        const t_complex weight1, const t_real, const t_real, const t_real,
                        const t_complex vis2, const t_complex weight2) {
                       return (weight1.real() > 0.) and (weight2.real() > 0.)
                              and (std::abs(vis1) > 1e-20) and (std::abs(vis2) > 1e-20)
                              and (!std::isnan(vis1.real()) and !std::isnan(vis1.imag()))
                              and (!std::isnan(vis2.real()) and !std::isnan(vis2.imag()));
                     });
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
//! read polarisation data from uvfits data
utilities::vis_params read_polarisation(const Vector<t_real> &data, const Matrix<t_real> &coords,
                                        const Vector<t_real> &frequencies, const t_uint pol_index1,
                                        const t_uint pols, const t_uint baselines,
                                        const t_uint channels);
//! read data from uvfits file
Vector<t_real> read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                                const t_int &baselines);
void read_uvfits_data(fitsfile *fptr, t_int *status, const std::vector<t_int> &naxis,
                      const t_int &baselines, Vector<t_real> &output);
//! read value from data
t_real read_value_from_data(const Vector<t_real> &data, const t_uint col, const t_uint pol,
                            const t_uint pols, const t_uint chan, const t_uint chans,
                            const t_uint baseline, const t_uint baselines);
//! return visibility for given baseline
t_complex read_vis_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                             const t_uint chan, const t_uint chans, const t_uint baseline,
                             const t_uint baselines);
//! return weight for given baseline
t_complex read_weight_from_data(const Vector<t_real> &data, const t_uint pol, const t_uint pols,
                                const t_uint chan, const t_uint chans, const t_uint baseline,
                                const t_uint baselines);
} // namespace pfitsio
} // namespace purify

#endif
