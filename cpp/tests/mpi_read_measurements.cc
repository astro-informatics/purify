#include "purify/config.h"
#include "purify/types.h"
#include "catch2/catch_all.hpp"
#include "purify/logging.h"

#include <iostream>
#include "purify/directories.h"
#include "purify/read_measurements.h"
#ifdef PURIFY_H5
#include "purify/h5reader.h"
#endif

using namespace purify;
using namespace purify::notinstalled;

TEST_CASE("uvfits") {
  auto const comm = sopt::mpi::Communicator::World();
  const std::string filename = atca_filename("0332-391");
  SECTION("one") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(filename + ".uvfits", comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886);
    }
    SECTION("vis") {
      const auto vis =
          read_measurements::read_measurements(filename + ".vis", comm, distribute::plan::w_term,
                                               false, stokes::I, utilities::vis_units::radians);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(vis.size()) == 245886);
      CHECK(vis.units == utilities::vis_units::radians);
    }
  }
  SECTION("two") {
    SECTION("uvfits") {
      const auto uvfits = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".uvfits", filename + ".uvfits"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886 * 2);
    }
    SECTION("vis") {
      const auto vis = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".vis", filename + ".vis"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(vis.size()) == 245886 * 2);
      CHECK(vis.units == utilities::vis_units::lambda);
    }
  }
  SECTION("ms") {
    SECTION("one") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(filename + ".ms", comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(ms.size()) == 245994);
#endif
    }
    SECTION("two") {
#ifdef PURIFY_CASACORE
      const auto ms = read_measurements::read_measurements(
          std::vector<std::string>{filename + ".ms", filename + ".ms"}, comm);
      CAPTURE(comm.rank());
      CHECK(comm.all_sum_all(ms.size()) == 245994 * 2);
#endif
    }
  }
  SECTION("H5") {
    SECTION("one") {
#ifdef PURIFY_H5
      // each rank reads the full file
      H5::H5Handler f(filename + ".h5");
      const std::vector<double> u = f.read("u");
      CAPTURE(u.size());
      // total size is Nranks * data length
      CHECK(comm.all_sum_all(u.size()) == 245886 * comm.size());
#endif
    }
    SECTION("two") {
#ifdef PURIFY_H5
      // each rank reads an evenly distributed slice of the data set
      H5::H5Handler f(filename + ".h5", comm);
      const std::vector<double> u = f.distread("u");
      CAPTURE(u.size());
      // total size is the data length
      CHECK(comm.all_sum_all(u.size()) == 245886);
#endif
    }
    SECTION("three") {
#ifdef PURIFY_H5
      // each rank reads a stochastically sampled set of 10k dataset members
      const size_t N = 10000;
      H5::H5Handler f(filename + ".h5", comm);
      const std::vector<double> u = f.stochread("u", N);
      CAPTURE(u.size());
      // total size is the data length
      CHECK(comm.all_sum_all(u.size()) == N * comm.size());
#endif
    }
    SECTION("four") {
#ifdef PURIFY_H5
      // Root rank reads the data and scatters evenly split slices
      const auto uvfits = read_measurements::read_measurements(filename + ".h5", comm);
      CAPTURE(uvfits.size());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886);
#endif
    }
    SECTION("five") {
#ifdef PURIFY_H5
      // each rank reads a stochastically sampled set of 10k dataset members
      // and constructs a uv_params object from it
      const size_t N = 10000;
      H5::H5Handler f(filename + ".h5", comm);
      const auto uvfits = H5::stochread_visibility(f, N, true);
      CAPTURE(uvfits.size());
      CHECK(comm.all_sum_all(uvfits.size()) == N * comm.size());
#endif
    }
  }
}
