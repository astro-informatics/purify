#ifndef PURIFY_UTILITIES_H
#define PURIFY_UTILITIES_H

#include "types.h"

#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace purify {

 namespace utilities {
      struct vis_params {
        Vector<t_real> u; // u coordinates
        Vector<t_real> v; // v coordinates
        Vector<t_real> w;
        Vector<t_complex> vis; // complex visiblities
        Vector<t_complex> weights; // weights for visibilities
        std::string units = "lambda";
      };
      //! Generates a random visibility coverage
      utilities::vis_params random_sample_density(const t_int& vis_num, const t_real& mean, const t_real& standard_deviation);
      //! Reads in visibility file
      utilities::vis_params read_visibility(const std::string& vis_name, const bool w_term = false);
      //! Writes visibilities to txt
      void write_visibility(const utilities::vis_params& uv_vis, const std::string & file_name, const bool w_term = false);
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
      Vector<t_int> ind2sub(const t_int& sub, const t_int& cols, const t_int& rows);
      //! Mod function modified to wrap circularly for negative numbers
      t_real mod(const t_real& x, const t_real& y);
      //! Calculate mean of vector
      t_complex mean(const Vector<t_complex> x);
      //! Calculate variance of vector
      t_real variance(const Vector<t_complex> x);
      //! Calculates the standard deviation of a vector
      t_real standard_deviation(const Vector<t_complex> x);
      //! Calculates the convolution between two images
      Image<t_complex> convolution_operator(const Image<t_complex>& a, const Image<t_complex>& b);
      //! Calculates upsample ratio for w-projection
      t_real upsample_ratio(const utilities::vis_params& uv_vis, const t_real& cell_x, const t_real& cell_y, const t_int& x_size, const t_int& y_size);
      //! Calculates convolution between grid and chirp matrix
      Sparse<t_complex> convolution(const Sparse<t_complex> & Grid, const Image<t_complex>& Chirp, const t_int& Nx, const t_int& Ny, const t_int& Nvis);
      //! zero pads ft grid for image up sampling and downsampling
      Matrix<t_complex> re_sample_ft_grid(const Matrix<t_complex>& input, const t_real& re_sample_factor);
      //! Sparsifies chirp
      Image<t_complex> sparsify_chirp(const Image<t_complex>& row, const t_real& energy);
      //! Generates image of chirp for w component
      Image<t_complex> generate_chirp(const t_real w_term, const t_real cellx, const t_real celly, const t_int x_size, const t_int y_size);
      //! A vector that whiten's the visibilities given the weights.
      utilities::vis_params whiten_vis(const utilities::vis_params& uv_vis);
      //! A function that calculates the l2 ball radius for sopt
      t_real calculate_l2_radius(const Vector<t_complex> & y);
 }
}

#endif