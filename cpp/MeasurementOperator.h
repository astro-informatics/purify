#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "types.h"

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
      //! Converts from subscript to index for matrix.
      t_int sub2ind(const t_int row, const t_int col, const t_int rows, const t_int cols);
      //! Converts from index to subscript for matrix.
      void ind2sub(const t_int sub, const t_int cols, const t_int rows, t_int* row, t_int* col);
      //! Match coordinates to grid
      Vector<t_real> omega_to_k(const Vector<t_real> omega);
      //! Write image to fits file
      void writefits2d(Image<t_real> image, std::string fits_name);
      //! Read image from fits file
      Image<t_real> readfits2d(std::string fits_name);
      //! Generates interpolation matrix from kernels
      Sparse<t_real> init_interpolation_matrix2d(const Vector<t_real> u, const Vector<t_real> v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generates scaling factors for gridding correction
      Image<t_real> init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv, const t_int ftsizeu, const t_int ftsizev);
      //! Generate gridding parameters, such as interpolation matrix
      MeasurementOperator::operator_params init_nufft2d(Vector<t_real> u, Vector<t_real> v, const t_int Ju, const t_int Jv, const t_int kernel, const t_int imsizex, const t_int imsizey, const t_int oversample_factor);
      //! Kaiser-Bessel kernel
      t_real kaiser_bessel(const t_real x, const t_int J); 
      //! Gaussian kernel
      t_real gaussian(const t_real x, const t_int J);
      //! Fourier transform of Gaussian kernel
      t_real ftgaussian(const t_real x, const t_int J);
    protected:
      Vector<> something_;
  };
}

#endif
