#include "catch2/catch_all.hpp"

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/read_measurements.h"

#include "purify/directories.h"
#include "purify/h5reader.h"

#include <vector>

using namespace purify;

TEST_CASE("Purify H5 reader", "[HDF5]") {
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

TEST_CASE("Purify H5 writer", "[HDF5]") {
  const auto uvfits = read_measurements::read_measurements(atca_filename("0332-391.uvfits"));

  H5::write_visibility(uvfits, "test-h5.h5", false);

  H5::H5Handler f("test-h5.h5");

  const std::vector<double> u = f.read("u");
  const std::vector<double> v = f.read("v");
  // const std::vector<double> w = f.read("w");
  const std::vector<double> re = f.read("re");
  const std::vector<double> im = f.read("im");
  const std::vector<double> sigma = f.read("sigma");

  CAPTURE(u.size());
  CAPTURE(v.size());
  // CAPTURE(w.size());
  CAPTURE(re.size());
  CAPTURE(im.size());
  CAPTURE(sigma.size());

  const bool pass = u.size() == uvfits.u.size() &&
                    u.size() == v.size() && /*u.size() == w.size() &&*/
                    u.size() == re.size() && u.size() == im.size() && u.size() == sigma.size();

  CHECK(pass);
}
