#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "types.h"
#include "utilities.h"
#include "FFTOperator.h"

#include <string>
#include <iostream>
#include <boost/math/special_functions/bessel.hpp>
#include <array>



namespace purify {

  //! This does something
  class MeasurementOperator {
   public:
      struct params {
        Sparse<t_complex> G;
        Image<t_real> S;
        t_int imsizex;
        t_int imsizey;
        t_int ftsizeu;
        t_int ftsizev;
      };

      //parameters needed for gridding and degridding function
      const params operator_params;
      
      
      MeasurementOperator(const Vector<t_real>& u, const Vector<t_real>& v, const t_int & Ju, const t_int & Jv, const std::string & kernel_name, const t_int & imsizex, const t_int & imsizey, const t_real & oversample_factor, const bool & fft_grid_correction = false)
      //  : u_{u}, v_{v}, oversample_factor_{oversample_factor}, imsizex_{imsizex}, imsizey_{imsizey};
      : operator_params(MeasurementOperator::init_nufft2d(u, v, Ju, Jv, kernel_name, imsizex, imsizey, oversample_factor, fft_grid_correction)) {};

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
      Fft2d fftop;
      //! Match uv coordinates to grid
      Vector<t_real> omega_to_k(const Vector<t_real>& omega);

      //! Generates interpolation matrix from kernels
      Sparse<t_complex> init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generates scaling factors for gridding correction using an fft
      Image<t_real> init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev, const t_int Ju, const t_int Jv);
      //! Generates scaling factors for gridding correction
      Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generate gridding parameters, such as interpolation matrix
      MeasurementOperator::params init_nufft2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::string kernel_name, const t_int imsizex, const t_int imsizey, const t_real oversample_factor, bool fft_grid_correction = false);
    public:
      //! Kaiser-Bessel kernel
      t_real kaiser_bessel(const t_real& x, const t_int& J);
      //! More general Kaiser-Bessel kernel
      t_real kaiser_bessel_general(const t_real& x, const t_int& J, const t_real& alpha);
      //! Fourier transform of kaiser bessel kernel
      t_real ft_kaiser_bessel(const t_real& x, const t_int& J);
      //! Gaussian kernel
      t_real gaussian(const t_real& x, const t_int& J);
      //! Fourier transform of Gaussian kernel
      t_real ft_gaussian(const t_real& x, const t_int& J);
      //! Performs calculations for PSWF and its Fourier Transform
      t_real calc_for_pswf(const t_real& x, const t_int& J, const t_real& alpha);
      //! PSWF kernel
      t_real pswf(const t_real& x, const t_int& J);
      //! Fourier transform of PSWF kernel
      t_real ft_pswf(const t_real& x, const t_int& J);
      //! Calculates samples of a kernel
      Vector<t_real> kernel_samples(const t_int& total_samples, const std::function<t_real(t_real)> kernelu, const t_int& J);
      //! linearly interpolates from samples of kernel
      t_real kernel_linear_interp(const Vector<t_real>& samples, const t_real& x, const t_int& J);

  };
}

#endif
