#include "purify/config.h"
#include "purify/types.h"
#include "catch2/catch_all.hpp"
#include "purify/logging.h"

#include "purify/directories.h"
#include "purify/read_measurements.h"
#include "purify/utilities.h"
#include <sopt/gradient_utils.h>
#ifdef PURIFY_H5
#include "purify/h5reader.h"
#include "purify/measurement_operator_factory.h"
#endif

using namespace purify;

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
#ifdef PURIFY_H5
  SECTION("H5") {
    SECTION("one") {
      // each rank reads the full file
      H5::H5Handler f(filename + ".h5");
      const std::vector<double> u = f.read("u");
      CAPTURE(u.size());
      // total size is Nranks * data length
      CHECK(comm.all_sum_all(u.size()) == 245886 * comm.size());
    }
    SECTION("two") {
      // each rank reads an evenly distributed slice of the data set
      H5::H5Handler f(filename + ".h5", comm);
      const std::vector<double> u = f.distread("u");
      CAPTURE(u.size());
      // total size is the data length
      CHECK(comm.all_sum_all(u.size()) == 245886);
    }
    SECTION("three") {
      // Root rank reads the data and scatters evenly split slices
      const auto uvfits = read_measurements::read_measurements(filename + ".h5", comm);
      CAPTURE(uvfits.size());
      CHECK(comm.all_sum_all(uvfits.size()) == 245886);
    }
    SECTION("four") {
      // each rank reads a stochastically sampled set of 10k dataset members
      // @todo account for w-stacking
      const size_t N = 10000;
      H5::H5Handler f(filename + ".h5", comm);
      const std::vector<double> u = f.stochread("u", N);
      CAPTURE(u.size());
      // total size is the data length
      CHECK(comm.all_sum_all(u.size()) == N * comm.size());
    }
    SECTION("five") {
      // each rank reads a stochastically sampled set of 10k dataset members
      // and constructs a uv_params object from it
      // @todo account for w-stacking
      const size_t N = 10000;
      H5::H5Handler f(filename + ".h5", comm);
      const auto uvfits = H5::stochread_visibility(f, N, true);  //< true = include w-term
      CAPTURE(uvfits.size());
      CHECK(comm.all_sum_all(uvfits.size()) == N * comm.size());
    }
    SECTION("six") {
      // a functor is used to read a stochastically sampled set of 10k dataset members
      // on each rank and to constructs a uv_params object from it, along with a measurement
      // operator which are then returned, wrapped in a sopt::IterationState object
      // @todo account for w-stacking
      const size_t N = 10000;
      H5::H5Handler h5file(filename + ".h5", comm);
      using t_complexVec = Vector<t_complex>;

      // This functor would be defined in Purify
      auto functor = [&f = h5file, &N]() {
        utilities::vis_params uv_data = H5::stochread_visibility(f, N, true);
        auto phi = factory::measurement_operator_factory<t_complexVec>(
            factory::distributed_measurement_operator::mpi_distribute_image, uv_data, 128, 128, 1,
            1, 2, kernels::kernel_from_string.at("kb"), 4, 4);

        return sopt::IterationState<t_complexVec>(uv_data.vis, phi);
      };

      // And it would be called in Sopt like this
      sopt::IterationState<t_complexVec> item = functor();

      // Make sure the return values are sensible
      const bool pass = comm.all_sum_all(item.target().size()) == N * comm.size() &&
                        item.phi().sizes()[0] == 0 && item.phi().sizes()[1] == 1 &&
                        item.phi().sizes()[2] == N;
      CHECK(pass);
    }
  }
#endif
}
