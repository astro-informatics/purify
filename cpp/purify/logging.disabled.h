#ifndef PURIFY_LOGGING_DISABLED_H
#define PURIFY_LOGGING_DISABLED_H

#include "purify/config.h"
#include <memory>
#include <string>

namespace purify {
namespace logging {
//! Name of the purify logger
const std::string name_prefix = "purify::";

inline std::shared_ptr<int> initialize(std::string const &) { return nullptr; }
inline std::shared_ptr<int> initialize() { return nullptr; }
inline std::shared_ptr<int> get(std::string const &) { return nullptr; }
inline std::shared_ptr<int> get() { return nullptr; }
inline void set_level(std::string const &, std::string const &){};
inline void set_level(std::string const &){};
inline bool has_level(std::string const &, std::string const &) { return false; }
}
}

//! \macro For internal use only
#define PURIFY_LOG_(...)

#endif
