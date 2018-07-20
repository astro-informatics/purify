#include <iomanip>
#include "catch.hpp"


#include "purify/kernels.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/directories.h"
#include "purify/test_data.h"
#include "purify/operators.h"

using namespace purify::notinstalled;
using namespace purify;

TEST_CASE("regression_degrid"){
  const std::string test_dir = "expected/measurement_operator/";
 const std::vector<t_real> u = read_data<t_real>(notinstalled::data_filename(test_dir + "u"));
 const std::vector<t_real> v = read_data<t_real>(notinstalled::data_filename(test_dir + "v"));
 std::string const fitsfile = image_filename("M31.fits");
  Image<t_complex> const M31 = pfitsio::read2d(fitsfile);
  Vector<t_complex> const input = Vector<t_complex>::Map(M31.data(), M31.size());
  const t_uint M = u.size();
  const t_real oversample_ratio = 2;
  const t_uint imsizex = M31.cols();
  const t_uint imsizey = M31.rows();
  const t_uint ftsizev = std::floor(imsizey * oversample_ratio);
  const t_uint ftsizeu = std::floor(imsizex * oversample_ratio);
  const t_uint Ju = 4;
  const t_uint Jv = 4;
  const t_uint power_iters = 1;
  const t_real power_tol = 1e-9;

SECTION("kb"){
  const kernels::kernel kernel = kernels::kernel::kb;
  const std::vector<t_complex> y_exact = read_data<t_complex>(notinstalled::data_filename(test_dir +"kb_exact_data"));
  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, Ju, Jv);
  const Vector<t_complex> y_test =  (*measure_op) * input;
  const Vector<t_complex> y = Vector<t_complex>::Map(y_exact.data(), y_exact.size());
  CAPTURE(y_test.array()/y.array());
  const t_real max_test = y_test.cwiseAbs().maxCoeff();
  const t_real max_data = y.cwiseAbs().maxCoeff();
  CAPTURE(y_test/max_test);
  CAPTURE(y/max_data);
  CHECK((y_test/max_test).isApprox((y/max_data), 1e-6));
}
SECTION("pswf"){
  const kernels::kernel kernel = kernels::kernel::pswf;
  const std::vector<t_complex> y_exact = read_data<t_complex>(notinstalled::data_filename(test_dir +"pswf_exact_data"));

  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, 6, 6);
  const Vector<t_complex> y_test =  (*measure_op) * input;
  const Vector<t_complex> y = Vector<t_complex>::Map(y_exact.data(), y_exact.size());
  CAPTURE(y_test.array()/y.array());
  const t_real max_test = y_test.cwiseAbs().maxCoeff();
  const t_real max_data = y.cwiseAbs().maxCoeff();
  CAPTURE(y_test/max_test);
  CAPTURE(y/max_data);
  CHECK((y_test/max_test).isApprox((y/max_data), 1e-6));
}
SECTION("gauss"){
  const kernels::kernel kernel = kernels::kernel::gauss;
  std::vector<t_complex> y_exact = read_data<t_complex>(notinstalled::data_filename(test_dir + "gauss_exact_data"));
  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, 6, 6);
  const Vector<t_complex> y_test =  (*measure_op) * input;
  const Vector<t_complex> y = Vector<t_complex>::Map(y_exact.data(), y_exact.size());
  CAPTURE(y_test.array()/y.array());
  const t_real max_test = y_test.cwiseAbs().maxCoeff();
  const t_real max_data = y.cwiseAbs().maxCoeff();
  CAPTURE(y_test/max_test);
  CAPTURE(y/max_data);
  CHECK((y_test/max_test).isApprox((y/max_data), 1e-6));
}
}

