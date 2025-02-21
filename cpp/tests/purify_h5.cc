#include "catch2/catch_all.hpp"

#include "purify/config.h"
#include "purify/types.h"
#include "purify/logging.h"
#include "purify/read_measurements.h"
#include "purify/uvw_utilities.h"

#include "purify/directories.h"
#include "purify/h5reader.h"

#include "purify/measurement_operator_factory.h"

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

TEST_CASE("SKA Mid data") {

  auto filename_small = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_60.txt");
  auto filename_large = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_6.txt");
  auto hdf5_filename_large = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_6.h5");
  auto hdf5_filename_small = data_filename("ska_mid/uvw_ska1mid197_simulation_12h_dt_60.h5");

  // Create a random image
  auto const imgSize = 128;
  Image<t_complex> const &image = Image<t_complex>::Random(imgSize, imgSize);
  const t_real cellsize = 1. / image.size() * 60. * 60.;
  
  auto uvw_data_small = utilities::read_visibility(filename_small, true, false);
  // Create measurement operator to generate measurements
  auto measurement_op_small = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uvw_data_small, image.rows(), image.cols(), cellsize, cellsize, 2, kernels::kernel::kb, 8, 8, false);
  // Generate measurements from image
  uvw_data_small.vis = (*measurement_op_small) * Image<t_complex>::Map(image.data(), image.size(), 1);
  // Write out visibilities
  H5::write_visibility(uvw_data_small, hdf5_filename_small, true);

  auto uvw_data_large = utilities::read_visibility(filename_large, true, false);
  // Create measurement operator to generate measurements
  auto measurement_op_large = measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uvw_data_large, image.rows(), image.cols(), cellsize, cellsize, 2, kernels::kernel::kb, 8, 8, false);  
  // Generate measurements from image
  uvw_data_large.vis = (*measurement_op_large) * Image<t_complex>::Map(image.data(), image.size(), 1);
  // Write out visibilities
  H5::write_visibility(uvw_data_large, hdf5_filename_large, true);

  
}
