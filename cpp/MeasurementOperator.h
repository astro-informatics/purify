#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "types.h"
#include "utilities.h"
#include "FFTOperator.h"
#include "kernels.h"

#include <CCfits/CCfits>
#include <string>
#include <iostream>




namespace purify {

  //! This does something
  class MeasurementOperator {
   public:
        Sparse<t_complex> G;
        Image<t_real> S;
        Array<t_complex> W;
        Image<t_complex> C;
        t_real norm = 1;
        const t_real oversample_factor;
        t_real resample_factor = 1;
        const bool use_w_term = false;
        const t_int imsizex;
        const t_int imsizey;
        t_int ftsizeu;
        t_int ftsizev;
      
      
      MeasurementOperator(const utilities::vis_params& uv_vis_input, const t_int & Ju, const t_int & Jv, 
        const std::string & kernel_name, const t_int & imsizex, const t_int & imsizey, const t_real & oversample_factor, 
        const t_real & cell_x = 1, const t_real & cell_y = 1, const std::string& weighting_type = "none", const t_real& R = 0, 
        bool use_w_term = false, const t_real & energy_fraction = 1, bool fft_grid_correction = false);
     
#   define SOPT_MACRO(NAME, TYPE)                                                          \
        TYPE const& NAME() const { return NAME ## _; }                                     \
        MeasurementOperator & NAME(TYPE const &NAME) { NAME ## _ = NAME; return *this; }  \
      protected:                                                                           \
        TYPE NAME ## _;                                                                    \
      public:



/*
     SOPT_MACRO(u, Vector<t_real>);
     SOPT_MACRO(v, Vector<t_real>);
     SOPT_MACRO(Ju, t_int);
     SOPT_MACRO(Jv, t_int);
     SOPT_MACRO(imsizex, t_int);
     SOPT_MACRO(imsizey, t_int);
     SOPT_MACRO(kernel_name, std::string);
     SOPT_MACRO(oversample_factor, t_real); 
     SOPT_MACRO(fft_grid_correction, bool);
*/
#     undef SOPT_MACRO
      //t_uint nx() const { return nx_; }
      //MeasurementOperator& nx(t_uint nx) { nx_ = nx; return *this; }


      //! Degridding operator that degrids image to visibilities
      Vector<t_complex> degrid(const Image<t_complex>& eigen_image);
      //! Gridding operator that grids image from visibilities
      Image<t_complex> grid(const Vector<t_complex>& visibilities);

    protected:
      FFTOperator fftop;
      //! Match uv coordinates to grid
      Vector<t_real> omega_to_k(const Vector<t_real>& omega);
      //! Generates interpolation matrix from kernels without using w-component
      Sparse<t_complex> init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, 
        const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv);
      //! Generates interpolation matrix from kernels with using w-coponent
      Sparse<t_complex> init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const Vector<t_real>& w, const t_int Ju, 
        const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv);      
      //! Generates scaling factors for gridding correction using an fft
      Image<t_real> init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int Ju, const t_int Jv);
      //! Generates scaling factors for gridding correction
      Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv);
      //! Generates and calculates weights
      Array<t_complex> init_weights(const Vector<t_real>& u, const Vector<t_real>& v, const Vector<t_complex>& weights, const t_real & oversample_factor, const std::string& weighting_type, const t_real& R);
      //! Estiamtes norm of operator
      t_real power_method(const t_int niters);
      //! Generates chirp matrix
      Matrix<t_complex> create_chirp_matrix(const Vector<t_real> & w_components, const t_real cell_x, const t_real cell_y, const t_real& energy_fraction = 1);

    public:
      //! Calculate column of covariance matrix for a given column/waveform
      Image<t_complex> covariance_calculation(const Image<t_complex> & vector);

  };
}

#endif
