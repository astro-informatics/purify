#ifndef PURIFY_LOGGING_H
#define PURIFY_LOGGING_H

#include "purify/config.h"
#include <exception>
#include <map>
#include <memory>
#include <tuple>
#include <sstream>
#include <string>

namespace purify::logging {

/// @brief Logging system for controlled & formatted writing to stdout
class Log {
public:

  /// Log priority levels.
  enum Level {
    trace = 0,
    debug = 10,
    info = 20,
    warn = 30, warning = 30,
    error = 40,
    critical = 50, always = 50
  };
  static const int end_color = -10; ///< Special "level-like" code to end coloring


  /// Typedef for a collection of named logs.
  using LogMap = std::map<std::string, Log>;

  /// Typedef for a collection of named log levels.
  using LevelMap = std::map<std::string, int>;

  /// @brief Typedef for a collection of shell color codes, accessed by log level.
  using ColorCodes = std::map<int, std::string>;


private:

  /// A static map of existing logs: we don't make more loggers than necessary.
  thread_local static LogMap existingLogs;

  /// A static map of default log levels.
  thread_local static LevelMap defaultLevels;

  /// Show timestamp?
  static bool showTimestamp;

  /// Show log level?
  static bool showLogLevel;

  /// Show logger name?
  static bool showLoggerName;

  /// Use shell colour escape codes?
  static bool useShellColors;


public:

  /// Set the log levels
  static void setLevel(const std::string& name, int level);
  static void setLevels(const LevelMap& logLevels);

protected:

  /// @name Hidden constructors etc.
  /// @{

  /// Constructor 1
  Log(const std::string& name);

  /// Constructor 2
  Log(const std::string& name, int level);

  /// @}

  /// @brief Get the TTY code string for coloured messages
  static std::string getColorCode(int level);


public:

  /// Get a logger with the given name. The level will be taken from the
  /// "requestedLevels" static map or will be INFO by default.
  static Log& getLog(const std::string& name);

  /// Get the priority level of this logger.
  int getLevel() const {
    return _level;
  }

  /// Set the priority level of this logger.
  Log& setLevel(int level) {
    _level = level;
    return *this;
  }

  /// Get a log level enum from a string.
  static Level getLevelFromName(const std::string& level);

  /// Get the std::string representation of a log level.
  static std::string getLevelName(int level);

  /// Get the name of this logger.
  std::string getName() const {
    return _name;
  }

  /// Set the name of this logger.
  Log& setName(const std::string& name) {
    _name = name;
    return *this;
  }

  /// Will this log level produce output on this logger at the moment?
  bool isActive(int level) const {
    return (level >= _level);
  }


private:

  /// This logger's name
  std::string _name;

  /// Threshold level for this logger.
  int _level;

protected:

  /// Write a message at a particular level.
  void log(int level, const std::string& message);

  /// Turn a message string into the current log format.
  std::string formatMessage(int level, const std::string& message);

public:

  /// The streaming operator can use Log's internals.
  friend std::ostream& operator<<(Log& log, int level);

};


/// Streaming output to a logger must have a Log::Level/int as its first argument.
std::ostream& operator<<(Log& log, int level);

/// Access method to default Log object
inline Log& getLog() { return Log::getLog("purify::"); }

/// Method to set the logging level of the default Log object
inline void set_level(const std::string &level) {
  getLog().setLevel(Log::getLevelFromName(level));
}


/// Helper method to ireplace a set of curly braces with
/// the template argument @a arg in a string stream
template <typename Arg>
void applyFormat(std::stringstream& ss, char*& pos, Arg&& arg) {
  char* delim = strstr(pos, "{}");
  if (delim != NULL) {
    ss << std::string(pos, delim-pos) << std::forward<Arg>(arg);
    pos = delim + 2;
  }
  else {
    throw std::runtime_error("Insufficient placeholders for number of arguments!");
  }
}

/// Helper method to construct formatted string
template <typename... Args>
inline std::string mkFormattedString(const char* txt, Args&& ... args) {
  std::string mys = txt;
  std::stringstream rtn;
  char* pos = (char*)txt;
  ((void)applyFormat(rtn, pos, std::forward<Args>(args)), ...);
  rtn << std::string(pos);
  return rtn.str();
}

inline std::string mkFormattedString(const std::string& txt) {
  return mkFormattedString(txt.data());
}

/// @defgroup logmacros Logging macros
/// @{

/// @def PURIFY_MSG_LVL
/// @brief Neat CPU-conserving logging macros. Use by preference!
/// @note Only usable in classes where a getLog() method is provided
#define PURIFY_MSG_LVL(lvl, ...) \
  do { \
    if (purify::logging::getLog().isActive(lvl)) { \
      purify::logging::getLog() << lvl << purify::logging::mkFormattedString(__VA_ARGS__) << '\n'; \
    } \
  } while (0)

//! \macro For internal use only
#define PURIFY_LOG_(TYPE, ...) \
  PURIFY_MSG_LVL(purify::logging::Log::TYPE, __VA_ARGS__)


/// @}

}

//! \macro Normal but signigicant condition
#define PURIFY_CRITICAL(...) PURIFY_LOG_(critical, __VA_ARGS__)
//! \macro Something is definitely wrong, algorithm exits
#define PURIFY_ERROR(...) PURIFY_LOG_(error, __VA_ARGS__)
//! \macro Something might be going wrong
#define PURIFY_WARN(...) PURIFY_LOG_(warn, __VA_ARGS__)
//! \macro Informational message about normal condition
//! \details Say "Residuals == "
#define PURIFY_INFO(...) PURIFY_LOG_(info, __VA_ARGS__)
//! \macro Output some debugging
#define PURIFY_DEBUG(...) PURIFY_LOG_(debug, __VA_ARGS__)
//! \macro Output internal values of no interest to anyone
//! \details Except maybe when debugging.
#define PURIFY_TRACE(...) PURIFY_LOG_(trace, __VA_ARGS__)

//! High priority message
#define PURIFY_HIGH_LOG(...) PURIFY_LOG_(critical, __VA_ARGS__)
//! Medium priority message
#define PURIFY_MEDIUM_LOG(...) PURIFY_LOG_(info, __VA_ARGS__)
//! Low priority message
#define PURIFY_LOW_LOG(...) PURIFY_LOG_(debug, __VA_ARGS__)

#endif
