#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "types.h"

#include <CCfits>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <boost/math/special_functions/bessel.hpp>

namespace purify {

  //! This does something
  class MeasurementOperator {
    public:
      struct operator_params {
        Sparse<t_real> G;
        Image<t_real> S;
        t_int imsizex;
        t_int imsizey;
        t_int ftsizeu;
        t_int ftsizev;
      };
      struct vis_params {
        Vector<t_real> u;
        Vector<t_real> v;
        Vector<t_complex> vis;
      };
      //! Reads in visibility file
      MeasurementOperator::vis_params read_visibility(const std::string& vis_name);
      //! scales the visibilities to units of pixels
      MeasurementOperator::vis_params uv_scale(const MeasurementOperator::vis_params& uv_vis, const t_int& ftsizeu, const t_int& ftsizev);
      //! Puts in conjugate visibilities
      MeasurementOperator::vis_params uv_symmetry(const MeasurementOperator::vis_params& uv_vis);
      //! Performs fftshift on 1d vector
      Vector<t_complex> fftshift_1d(Vector<t_complex> input);
      //! Performs fftshift on 2d matrix
      Matrix<t_complex> fftshift_2d(Matrix<t_complex> input);
      //! Performs fftshift on 2d matrix
      Matrix<t_complex> ifftshift_2d(Matrix<t_complex> input);
      //! Performs ifftshift on 1d vector
      Vector<t_complex> ifftshift_1d(Vector<t_complex> input);
      //! Degridding operator that degrids image to visibilities
      Vector<t_complex> degrid(const Image<t_complex>& eigen_image, const MeasurementOperator::operator_params st);
      //! Gridding operator that grids image from visibilities
      Image<t_complex> grid(const Vector<t_complex>& visibilities, const MeasurementOperator::operator_params st);
      //! Uses Eigen's 1D FFT to perform 2D FFT
      Matrix<t_complex> fft2d(const Matrix<t_complex>& input);
      //! Uses Eigen's 1D IFFT to perform 2D IFFT
      Matrix<t_complex> ifft2d(const Matrix<t_complex>& input);
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
      //! Generates interpolation matrix from kernels
      Sparse<t_real> init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generates scaling factors for gridding correction using an fft
      Image<t_real> init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev, const t_int Ju, const t_int Jv);
      //! Generates scaling factors for gridding correction
      Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generate gridding parameters, such as interpolation matrix
      MeasurementOperator::operator_params init_nufft2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::string kernel_name, const t_int imsizex, const t_int imsizey, const t_int oversample_factor);
      //! Kaiser-Bessel kernel
      t_real kaiser_bessel(const t_real& x, const t_int& J);
      //! Fourier transform of kaiser bessel kernel
      t_real ft_kaiser_bessel(const t_real& x, const t_int& J);
      //! Gaussian kernel
      t_real gaussian(const t_real& x, const t_int& J);
      //! Fourier transform of Gaussian kernel
      t_real ft_gaussian(const t_real& x, const t_int& J);
      //! PSWF kernel
      t_real pswf(const t_real& x, const t_int& J);
      //! Fourier transform of PSWF kernel
      t_real ft_pswf(const t_real& x, const t_int& J);
      //! Calculates samples of a kernel
      Vector<t_real> kernel_samples(const t_int& total_samples, const std::function<t_real(t_real)> kernelu, const t_int& J);
      //! linearly interpolates from samples of kernel
      t_real kernel_linear_interp(const Vector<t_real>& samples, const t_real& x, const t_int& J);
    protected:
      Vector<> something_;
    private:
      //polynomial coefficients for prolate spheriodal wave function rational approximation
      t_real p1[6] = {8.203343e-2, -3.644705e-1, 6.278660e-1, -5.335581e-1, 2.312756e-1, 2*0.0};
      t_real p2[6] = {4.028559e-3, -3.697768e-2, 1.021332e-1, -1.201436e-1, 6.412774e-2, 2*0.0};
      t_real q1[3] = {1., 8.212018e-1, 2.078043e-1};
      t_real q2[3] = {1., 9.599102e-1, 2.918724e-1};
  };
}

#endif
