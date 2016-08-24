#ifndef PURIFY_CPP_CONFIG_H
#define PURIFY_CPP_CONFIG_H

//! Problems with using and constructors
#cmakedefine PURIFY_HAS_USING
#ifndef PURIFY_HAS_USING
#define PURIFY_HAS_NOT_USING
#endif

//! Whether to do logging or not
#cmakedefine PURIFY_DO_LOGGING

//! Whether to do openmp
#cmakedefine PURIFY_OPENMP

#include <string>
#include <tuple>

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
//! Default logger name
inline std::string default_logger_name() { return "@PURIFY_LOGGER_NAME@"; }
//! Wether to add color to the logger
inline constexpr bool color_logger() {
  // clang-format off
  return @PURIFY_COLOR_LOGGING@;
  // clang-format on
}
}

#endif
