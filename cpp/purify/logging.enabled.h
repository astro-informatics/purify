#ifndef PURIFY_LOGGING_ENABLED_H
#define PURIFY_LOGGING_ENABLED_H

#include "purify/config.h"
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

namespace purify {
namespace logging {
void set_level(std::string const &level, std::string const &name = "");

//! \brief Initializes a logger.
//! \details Logger only exists as long as return is kept alive.
inline std::shared_ptr<spdlog::logger> initialize(std::string const &name = "") {
  auto const result = spdlog::stdout_logger_mt(default_logger_name() + name);
  set_level(default_logging_level(), name);
  return result;
}

//! Returns shared pointer to logger or null if it does not exist
inline std::shared_ptr<spdlog::logger> get(std::string const &name = "") {
  return spdlog::get(default_logger_name() + name);
}

//! \brief Sets loggin level
//! \details Levels can be one of
//!     - "trace"
//!     - "debug"
//!     - "info"
//!     - "warn"
//!     - "err"
//!     - "critical"
//!     - "off"
inline void set_level(std::string const &level, std::string const &name) {
  auto const logger = get(name);
  if (not logger) throw std::runtime_error("No logger by the name of " + std::string(name));
#define PURIFY_MACRO(LEVEL) \
  if (level == #LEVEL) logger->set_level(spdlog::level::LEVEL)
  PURIFY_MACRO(trace);
  else PURIFY_MACRO(debug);
  else PURIFY_MACRO(info);
  else PURIFY_MACRO(warn);
  else PURIFY_MACRO(err);
  else PURIFY_MACRO(critical);
  else PURIFY_MACRO(off);
#undef PURIFY_MACRO
  else throw std::runtime_error("Unknown logging level " + std::string(level));
}

inline bool has_level(std::string const &level, std::string const &name = "") {
  auto const logger = get(name);
  if (not logger) return false;

#define PURIFY_MACRO(LEVEL) \
  if (level == #LEVEL) return logger->level() >= spdlog::level::LEVEL
  PURIFY_MACRO(trace);
  else PURIFY_MACRO(debug);
  else PURIFY_MACRO(info);
  else PURIFY_MACRO(warn);
  else PURIFY_MACRO(err);
  else PURIFY_MACRO(critical);
  else PURIFY_MACRO(off);
#undef PURIFY_MACRO
  else throw std::runtime_error("Unknown logging level " + std::string(level));
}
}  // namespace logging
}  // namespace purify

//! \macro For internal use only
#define PURIFY_LOG_(NAME, TYPE, ...)                                            \
  if (auto purify_logging_##__func__##_##__LINE__ = purify::logging::get(NAME)) \
  purify_logging_##__func__##_##__LINE__->TYPE(__VA_ARGS__)

#endif
