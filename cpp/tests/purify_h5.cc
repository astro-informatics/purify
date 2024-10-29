#include "catch2/catch_all.hpp"

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"

#include "purify/directories.h"
#include "purify/h5reader.h"

#include <vector>

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("Purify H5", "[HDF5]") {
  H5::H5Handler f(atca_filename("0332-391.h5"));

  const std::vector<double> u = f.read("u");
  const std::vector<double> v = f.read("v");
  const std::vector<double> w = f.read("w");
  const std::vector<double> re = f.read("re");
  const std::vector<double> im = f.read("im");
  const std::vector<double> sigma = f.read("sigma");

  CAPTURE(u.size());
  CAPTURE(v.size());
  CAPTURE(w.size());
  CAPTURE(re.size());
  CAPTURE(im.size());
  CAPTURE(sigma.size());

  const bool pass = u.size() > 0 && u.size() == v.size() && u.size() == w.size() &&
                    u.size() == re.size() && u.size() == im.size() && u.size() == sigma.size();
  CHECK(pass);
}
