#ifndef PURIFY_PFITSIO_H
#define PURIFY_PFITSIO_H

#include "types.h"

#include <CCfits>
#include "utilities.h"


namespace purify {

 namespace pfitsio {
      //! Write image to fits file
      void write2d(const Image<t_real> & image, const std::string& fits_name, const bool& overwrite, const bool& flip);
      //! Read image from fits file
      Image<t_complex> read2d(const std::string& fits_name);
 }
}

#endif