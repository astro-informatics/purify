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
       Sparse<t_complex> G;
        Image<t_real> S;
        const t_int imsizex;
        const t_int imsizey;
        const t_int ftsizeu;
        const t_int ftsizev;
      
      
      
      MeasurementOperator(const Vector<t_real>& u, const Vector<t_real>& v, const t_int & Ju, const t_int & Jv, const std::string & kernel_name, const t_int & imsizex, const t_int & imsizey, const t_real & oversample_factor, bool fft_grid_correction = false);
     
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
      Sparse<t_complex> init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv);
      //! Generates scaling factors for gridding correction using an fft
      Image<t_real> init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int Ju, const t_int Jv);
      //! Generates scaling factors for gridding correction
      Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv);
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
      //! \brief Calculates Horner's Rule the standard PSWF for radio astronomy, with a support of J = 6 and alpha = 1.
      //! \param[in] eta0: value to evaluate
      //! \param[in] J: support size of gridding kernel
      //! \param[in] alpha: type of special PSWF to calculate
      //! \details The tailored prolate spheroidal wave functions for gridding radio astronomy.
      //! Details are explained in Optimal Gridding of Visibility Data in Radio
      //! Astronomy, F. R. Schwab 1983.
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
