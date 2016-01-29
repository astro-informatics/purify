#include "catch.hpp"
#include "utilities.h"
#include "pfitsio.h"
#include "FFTOperator.h"
using namespace purify;

TEST_CASE("FFT Operator [FORWARD]", "[FORWARD]") {

  Fft2d oldFFT;
  FFTOperator newFFT;
  Matrix<t_complex> a = Matrix<t_complex>::Random(20, 20);

  Matrix<t_complex> old_output = oldFFT.forward(a);
  Matrix<t_complex> new_output = newFFT.forward(a);
  CHECK(old_output.isApprox(new_output, 1e-13));
  CHECK(std::abs(old_output(0)/new_output(0) - old_output(1)/new_output(1)) < 1e-13);
  Vector<t_real> x = Vector<t_real>::LinSpaced(128, -2, 2);
  Matrix<t_complex> guassian(128, 128);
  for (int i = 0; i < x.size(); ++i)
  {
    for (int j = 0; j < x.size(); ++j)
    {
      t_real sigma = 0.1;
      t_complex I(0, 1);
      guassian(i, j) = std::exp(- (x(i) * x(i) + x(j) * x(j)) * 0.5/(sigma * sigma) -2. * I * purify_pi * 64. * (x(j) + x(i)));
    }
  }
  //pfitsio::write2d(newFFT.forward(guassian).real(), "forward_guassian.fits", true, false);
}

TEST_CASE("FFT Operator [INVERSE]", "[INVERSE]") {

  Fft2d oldFFT;
  FFTOperator newFFT;
  Matrix<t_complex> a = Matrix<t_complex>::Random(20, 20);

  Matrix<t_complex> old_output = oldFFT.inverse(a);
  Matrix<t_complex> new_output = newFFT.inverse(a);
  CHECK(old_output.isApprox(new_output, 1e-13));
  CHECK(std::abs(old_output(0)/new_output(0) - old_output(1)/new_output(1)) < 1e-13);

  Vector<t_real> x = Vector<t_real>::LinSpaced(128, -2, 2);
  Matrix<t_complex> guassian(128, 128);
  for (int i = 0; i < x.size(); ++i)
  {
    for (int j = 0; j < x.size(); ++j)
    {
      t_real sigma = 0.1;
      t_complex I(0, 1);
      guassian(i, j) = std::exp(- (x(i) * x(i) + x(j) * x(j)) * 0.5/(sigma * sigma) -2. * I * purify_pi * 64. * (x(j) + x(i)));
    }
  }
  //pfitsio::write2d(newFFT.inverse(guassian).real(), "inverse_guassian.fits", true, false);

}

TEST_CASE("FFT Operator [BOTH]", "[BOTH]") {

  FFTOperator newFFT;
  Matrix<t_complex> a = Matrix<t_complex>::Random(20, 20);

  Matrix<t_complex> new_output = newFFT.forward(newFFT.inverse(a));
  CHECK(a.isApprox(new_output));

  Vector<t_real> x = Vector<t_real>::LinSpaced(128, -2, 2);
  Matrix<t_complex> guassian(128, 128);
  for (int i = 0; i < x.size(); ++i)
  {
    for (int j = 0; j < x.size(); ++j)
    {
      t_real sigma = 0.1;
      guassian(i, j) = std::exp(- (x(i) * x(i) + x(j) * x(j)) * 0.5/(sigma * sigma) );
    }
  }
  pfitsio::write2d(newFFT.forward(newFFT.inverse(guassian)).real(), "guassian.fits", true, false);

}
