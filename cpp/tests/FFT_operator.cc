#include "catch.hpp"
#include "purify/FFTOperator.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
using namespace purify;

TEST_CASE("FFT Operator [FORWARD]", "[FORWARD]") {

  t_int fft_flag = (FFTW_PATIENT | FFTW_PRESERVE_INPUT);
  Fft2d oldFFT;
  auto newFFT = purify::FFTOperator().fftw_flag(fft_flag);
  CAPTURE(newFFT.fftw_flag());
  CAPTURE(fft_flag);
  newFFT.set_up_multithread();
  Matrix<t_complex> const a = Matrix<t_complex>::Random(20, 19);
  Matrix<t_complex> old_output = oldFFT.forward(a);
  Matrix<t_complex> new_output = newFFT.forward(a);
  CAPTURE(old_output.row(0).head(4));
  CAPTURE(new_output.row(0).head(4));
  CAPTURE(new_output.transpose().row(0).head(4));
  CAPTURE((&new_output(0, 0) + 20 == &new_output(0, 1)));
  CAPTURE((&new_output(0, 0) + 1 == &new_output(1, 0)));
  CHECK(old_output.isApprox(new_output, 1e-13));
  CHECK(std::abs(old_output(0) / new_output(0) - old_output(1) / new_output(1)) < 1e-13);

  t_int xsize = 128;
  t_int ysize = 100;

  Vector<t_real> x = Vector<t_real>::LinSpaced(xsize, -2, 2);
  Vector<t_real> y = Vector<t_real>::LinSpaced(ysize, -2, 2);
  Matrix<t_complex> guassian(ysize, xsize);
  for(int i = 0; i < y.size(); ++i) {
    for(int j = 0; j < x.size(); ++j) {
      t_real sigma = 0.1;
      t_complex I(0, 1);
      guassian(i, j) = std::exp(-(y(i) * y(i) + x(j) * x(j)) * 0.5 / (sigma * sigma)
                                - 2. * I * constant::pi * 64. * (x(j) + y(i)));
    }
  }
  pfitsio::write2d(newFFT.forward(guassian).cwiseAbs(), "forward_guassian.fits");
  pfitsio::write2d(oldFFT.forward(guassian).cwiseAbs(), "old_forward_guassian.fits");
}

TEST_CASE("FFT Operator [INVERSE]", "[INVERSE]") {

  Fft2d oldFFT;
  t_int fft_flag = (FFTW_PATIENT | FFTW_PRESERVE_INPUT);
  auto newFFT = purify::FFTOperator().fftw_flag(fft_flag);
  Matrix<t_complex> a = Matrix<t_complex>::Random(20, 19);

  Matrix<t_complex> old_output = oldFFT.inverse(a);
  Matrix<t_complex> new_output = newFFT.inverse(a);
  CHECK(old_output.isApprox(new_output, 1e-13));
  CHECK(std::abs(old_output(0) / new_output(0) - old_output(1) / new_output(1)) < 1e-13);
  t_int xsize = 128;
  t_int ysize = 100;

  Vector<t_real> x = Vector<t_real>::LinSpaced(xsize, -2, 2);
  Vector<t_real> y = Vector<t_real>::LinSpaced(ysize, -2, 2);
  Matrix<t_complex> guassian(ysize, xsize);
  for(int i = 0; i < y.size(); ++i) {
    for(int j = 0; j < x.size(); ++j) {
      t_real sigma = 0.1;
      t_complex I(0, 1);
      guassian(i, j) = std::exp(-(y(i) * y(i) + x(j) * x(j)) * 0.5 / (sigma * sigma)
                                - 2. * I * constant::pi * 64. * (x(j) + y(i)));
    }
  }
  pfitsio::write2d(newFFT.inverse(guassian).real(), "inverse_guassian.fits");
  pfitsio::write2d(oldFFT.inverse(guassian).real(), "old_inverse_guassian.fits");
}

TEST_CASE("FFT Operator [BOTH]", "[BOTH]") {

  Fft2d oldFFT;
  t_int fft_flag = (FFTW_PATIENT | FFTW_PRESERVE_INPUT);
  auto newFFT = purify::FFTOperator().fftw_flag(fft_flag);
  Matrix<t_complex> a = Matrix<t_complex>::Random(20, 19);

  Matrix<t_complex> new_output = newFFT.forward(newFFT.inverse(a));
  CHECK(a.isApprox(new_output, 1e-13));

  t_int xsize = 128;
  t_int ysize = 100;

  Vector<t_real> x = Vector<t_real>::LinSpaced(xsize, -2, 2);
  Vector<t_real> y = Vector<t_real>::LinSpaced(ysize, -2, 2);
  Matrix<t_complex> guassian(ysize, xsize);
  for(int i = 0; i < y.size(); ++i) {
    for(int j = 0; j < x.size(); ++j) {
      t_real sigma = 0.1;
      t_complex I(0, 1);
      guassian(i, j) = std::exp(-(y(i) * y(i) + x(j) * x(j)) * 0.5 / (sigma * sigma)
                                - 2. * I * constant::pi * 64. * (x(j) + y(i)));
    }
  }
  pfitsio::write2d(newFFT.forward(newFFT.inverse(guassian)).real(), "guassian.fits");
  pfitsio::write2d(oldFFT.forward(oldFFT.inverse(guassian)).real(), "old_guassian.fits");
}
