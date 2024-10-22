#ifndef PURIFY_CPP_CONFIG_H
#define PURIFY_CPP_CONFIG_H

//! Problems with using and constructors
#cmakedefine PURIFY_HAS_USING
#ifndef PURIFY_HAS_USING
#define PURIFY_HAS_NOT_USING
#endif

//! Whether to do openmp
#cmakedefine PURIFY_OPENMP

//! Whether FFTW has openmp
#cmakedefine PURIFY_OPENMP_FFTW

//! Whether PURIFY is running with mpi
#cmakedefine PURIFY_MPI

//! Whether PURIFY is running with arrayfire
#cmakedefine PURIFY_ARRAYFIRE

//! Whether PURIFY is running with CImg
#cmakedefine PURIFY_CImg

//! Whether PURIFY is running with casacore
#cmakedefine PURIFY_CASACORE

//! Whether PURIFY is using (and SOPT was built with) onnxrt support
#cmakedefine PURIFY_ONNXRT

//! Whether PURIFY is using HDF5 support
#cmakedefine PURIFY_H5

#include <string>
#include <tuple>
#include <cstdint>

namespace purify {
//! Returns library version
inline std::string version() { return "@Purify_VERSION@"; }
//! Returns library version
inline std::tuple<uint8_t, uint8_t, uint8_t> version_tuple() {
  // clang-format off
  return std::tuple<uint8_t, uint8_t, uint8_t>(
      @Purify_VERSION_MAJOR@, @Purify_VERSION_MINOR@, @Purify_VERSION_PATCH@);
  // clang-format on
}
//! Returns library git reference, if known
inline std::string gitref() { return "@Purify_GITREF@"; }
//! Default logging level
inline std::string default_logging_level() { return "@PURIFY_TEST_LOG_LEVEL@"; }
} // namespace purify

#endif
