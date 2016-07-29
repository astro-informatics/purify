#ifndef PURIFY_LOGGING_H
#define PURIFY_LOGGING_H

#include "config.h"

#ifdef PURIFY_DO_LOGGING
#include "logging.enabled.h"
#else
#include "logging.disabled.h"
#endif

//! \macro Informational message about normal condition
//! \details Say "Residuals == "
#define PURIFY_INFO(...) PURIFY_LOG_(, info, __VA_ARGS__)
//! \macro Normal but signigicant condition
#define PURIFY_NOTICE(...) PURIFY_LOG_(, critical, __VA_ARGS__)
//! \macro Something might be going wrong
#define PURIFY_WARN(...) PURIFY_LOG_(, warn, __VA_ARGS__)
//! \macro Something is definitely wrong, algorithm exits
#define PURIFY_ERROR(...) PURIFY_LOG_(, error, __VA_ARGS__)
//! \macro Output some debugging
#define PURIFY_DEBUG(...) PURIFY_LOG_(, debug, __VA_ARGS__)
//! \macro Output internal values of no interest to anyone
//! \details Except maybe when debugging.
#define PURIFY_TRACE(...) PURIFY_LOG_(, trace, __VA_ARGS__)

#endif
