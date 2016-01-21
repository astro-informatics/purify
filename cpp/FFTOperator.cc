
#include "FFTOperator.h"

namespace purify {

  Vector<t_complex> fftshift_1d(const Vector<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
  
    t_int input_size = input.size();
    Vector<t_complex> output(input_size);
    t_int NF = std::floor(input_size/2.0);
    t_int NC = std::ceil(input_size/2.0);

    
    for (t_int i=0; i<NF; i++)
      output(i)=input(i + NC);
    
    for (t_int i=NF; i<input_size; i++)
      output(i)=input(i - NF);

    return output;
  }

  Vector<t_complex> ifftshift_1d(Vector<t_complex> input) 
  {
    /*
      Performs a 1D ifftshift on a vector and returns the shifted vector

      input:: vector to perform ifftshift on.
    */
  
    t_int input_size = input.size();
    Vector<t_complex> output(input_size);
    t_int NF = std::floor(input_size/2.0);
    t_int NC = std::ceil(input_size/2.0);

    
    for (t_int i=0; i<NC; i++)
      output(i)=input(i + NF);
    
    for (t_int i=NC; i<input_size; i++)
      output(i)=input(i - NC);

    return output;
  }

 Matrix<t_complex> Fft2d::direct(const Matrix<t_complex>& input)
  {
    /*
      Returns FFT of a 2D matrix.

      input:: complex valued image
    */
    Eigen::FFT<t_real> fft;
    t_int dim_x = input.rows();
    t_int dim_y = input.cols();
    Matrix<t_complex> output(dim_x, dim_y);
    for (t_int k = 0; k < dim_x; k++) {
      Vector<t_complex> tmpOut(dim_y);
      Vector<t_complex> tmpIn = input.row(k);
      fft.fwd(tmpOut, tmpIn);
      output.row(k) = tmpOut;
    }

    for (t_int k = 0; k < dim_y; k++) {
      Vector<t_complex> tmpOut(dim_x);
      Vector<t_complex> tmpIn = output.col(k);
      fft.fwd(tmpOut, tmpIn);
      output.col(k) = tmpOut;
    }
    return output;
  }

  Matrix<t_complex> Fft2d::indirect(const Matrix<t_complex>& input)
  {
    /*
      Returns FFT of a 2D matrix.

      input:: complex valued image
    */
    Eigen::FFT<t_real> fft;
    t_int dim_x = input.rows();
    t_int dim_y = input.cols();
    Matrix<t_complex> output(dim_x, dim_y);
    for (t_int k = 0; k < dim_x; k++) {
      Vector<t_complex> tmpOut(dim_y);
      Vector<t_complex> tmpIn = input.row(k);
      fft.inv(tmpOut, tmpIn);
      output.row(k) = tmpOut;
    }

    for (t_int k = 0; k < dim_y; k++) {
      Vector<t_complex> tmpOut(dim_x);
      Vector<t_complex> tmpIn = output.col(k);
      fft.inv(tmpOut, tmpIn);
      output.col(k) = tmpOut;
    }
    return output;
  }


  Matrix<t_complex> Fft2d::shift(Matrix<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
      t_int rows = input.rows();
      t_int cols = input.cols();
      for (t_int i = 0; i < cols; ++i)
        input.col(i) = fftshift_1d(input.col(i));

      for (t_int i = 0; i < rows; ++i)
        input.row(i) = fftshift_1d(input.row(i));

      return input;
  }

  Matrix<t_complex> Fft2d::ishift(Matrix<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
      t_int rows = input.rows();
      t_int cols = input.cols();
      for (t_int i = 0; i < cols; ++i)
        input.col(i) = ifftshift_1d(input.col(i));

      for (t_int i = 0; i < rows; ++i)
        input.row(i) = ifftshift_1d(input.row(i));

      return input;
  }



}