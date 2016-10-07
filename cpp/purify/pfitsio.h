#ifndef PURIFY_PFITSIO_H
#define PURIFY_PFITSIO_H

#include "purify/config.h"
#include "purify/types.h"

#include <string>
#include <CCfits/CCfits>
#include "purify/utilities.h"

namespace purify {

namespace pfitsio {
struct header_params {
  // structure containing parameters for fits header
  std::string fits_name = "output_image.fits";
  t_real mean_frequency = 1400; // in MHz
  t_real cell_x = 1;            // in arcseconds
  t_real cell_y = 1;            // in arcseconds
  t_real ra = 0;                // in radians, converted to decimal degrees before write
  t_real dec = 0;               // in radians, converted to decimal degrees before write
  t_int pix_ref_x = 0;
  t_int pix_ref_y = 0;
  std::string pix_units = "Jy/BEAM";
  t_real channels_total = 1;
  t_real channel_width = 8; // in MHz
  t_real polarsiation = 1;
  t_int niters = 0; // number of iterations
  bool hasconverged = false; // stating if model has converged
  t_real relative_variation = 0;
  t_real residual_convergence = 0;
  t_real epsilon = 0;
};
//! Write image to fits file using header information
void write2d_header(const Image<t_real> &image, const pfitsio::header_params &header,
                    const bool &overwrite = true);
//! Write image to fits file
void write2d(const Image<t_real> &image, const std::string &fits_name,
             const std::string &pix_units = "Jy/Beam", const bool &overwrite = true);
//! Read image from fits file
Image<t_complex> read2d(const std::string &fits_name);
}
}

#endif
