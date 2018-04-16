#include <iomanip>
#include "catch.hpp"


#include "purify/kernels.h"
#include "purify/pfitsio.h"
#include "purify/utilities.h"
#include "purify/directories.h"
#include "purify/operators.h"

using namespace purify::notinstalled;
using namespace purify;

TEST_CASE("regression_degrid"){

  std::vector<t_real> u = {0.108592522700588,
0.834221624690559,
0.0921213228190870,
1.31037358629201,
1.22832235754686,
0.908160593478928,
0.725629190847630,
-0.0164711932171762,
1.20178117466350,
1.11972979041744};
  std::vector<t_real> v = {-1.00568176289943,
-0.654292329738481,
-0.491744281487334,
-1.65328204464114,
-0.847800474457697,
-1.58531846461720,
0.351389433160950,
0.513937503626511,
-0.647600348384954,
0.157881288441735};
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
  const auto ft_plan = operators::fftw_plan::measure;

SECTION("kb"){
  const kernels::kernel kernel = kernels::kernel::kb;
  std::vector<t_complex> y_exact = {
 t_complex(43.9571,0.451841),
 t_complex(42.5173,-11.367),
 t_complex(46.4067,-0.434311),
 t_complex(32.5041,-14.953),
 t_complex(37.8979,-16.144),
 t_complex(36.665,-9.93551),
 t_complex(43.4535,-11.6693),
 t_complex(46.3236,-0.794948),
 t_complex(38.8114,-16.2235),
 t_complex(39.9127,-16.4835)};

  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, Ju, Jv, ft_plan);
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
  std::vector<t_complex> y_exact = {
 t_complex(0.947241,0.0100152),
 t_complex(0.916021,-0.244937),
 t_complex(0.999958,-0.00919874),
 t_complex(0.700524,-0.322091),
 t_complex(0.816284,-0.347893),
 t_complex(0.790191,-0.213884),
 t_complex(0.936198,-0.251814),
 t_complex(0.998354,-0.0174951),
 t_complex(0.836104,-0.349782),
 t_complex(0.859524,-0.355529)};

  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, 6, 6, ft_plan);
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
  std::vector<t_complex> y_exact = {
 t_complex(0.946955,0.00976944),
 t_complex(0.916046,-0.244844),
 t_complex(0.999956,-0.00942444),
 t_complex(0.70044,-0.322196),
 t_complex(0.81619,-0.347937),
 t_complex(0.790183,-0.213866),
 t_complex(0.93622,-0.251604),
 t_complex(0.998348,-0.0172179),
 t_complex(0.836051,-0.34984),
 t_complex(0.859371,-0.355489)};

  const auto measure_op = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
        Vector<t_real>::Map(v.data(), v.size()), Vector<t_real>::Map(u.data(), u.size()),Vector<t_real>::Zero(M), Vector<t_complex>::Ones(M), imsizey, imsizex, oversample_ratio,
        power_iters, power_tol, kernel, 6, 6, ft_plan);
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

