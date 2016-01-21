#ifndef PURIFY_UTILITIES_H
#define PURIFY_UTILITIES_H

#include "types.h"

#include <CCfits>
#include <string>
#include <fstream>
#include <iostream>


namespace purify {

 namespace utilities {
      struct vis_params {
        Vector<t_real> u; // u coordinates
        Vector<t_real> v; // v coordinates
        Vector<t_complex> vis; // complex visiblities
        Vector<t_complex> weights; // weights for visibilities
      };
      //! Reads in visibility file
      utilities::vis_params read_visibility(const std::string& vis_name);
      //! Scales visibilities to a given pixel size in arcseconds
      utilities::vis_params set_cell_size(const utilities::vis_params& uv_vis, t_real cell_size_u = 0, t_real cell_size_v = 0);
      //! Apply weights to visiblities
      Vector<t_complex> apply_weights(const Vector<t_complex> visiblities, const Vector<t_complex> weights);
      //! scales the visibilities to units of pixels
      utilities::vis_params uv_scale(const utilities::vis_params& uv_vis, const t_int& ftsizeu, const t_int& ftsizev);
      //! Puts in conjugate visibilities
      utilities::vis_params uv_symmetry(const utilities::vis_params& uv_vis);
      //! Converts from subscript to index for matrix.
      t_int sub2ind(const t_int& row, const t_int& col, const t_int& rows, const t_int& cols);
      //! Converts from index to subscript for matrix.
      void ind2sub(const t_int sub, const t_int cols, const t_int rows, t_int* row, t_int* col);
      //! Mod function modified to wrap circularly for negative numbers
      t_int mod(const t_real& x, const t_real& y);
      //! Match coordinates to grid
      Vector<t_real> omega_to_k(const Vector<t_real>& omega);
      //! Write image to fits file
      void writefits2d(Image<t_real> image, const std::string& fits_name, const bool& overwrite, const bool& flip);
      //! Read image from fits file
      Image<t_complex> readfits2d(const std::string& fits_name);

 }
}

#endif