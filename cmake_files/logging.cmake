# Setup logging
set(PURIFY_LOGGER_NAME "purify" CACHE STRING "NAME of the logger")
set(PURIFY_COLOR_LOGGING true CACHE BOOL "Whether to add color to the log")
if(logging)
  set(PURIFY_DO_LOGGING 1)
  set(PURIFY_TEST_LOG_LEVEL critical CACHE STRING "Level when logging tests")
  set_property(CACHE PURIFY_TEST_LOG_LEVEL PROPERTY STRINGS
    off critical error warn info debug trace)
else()
  unset(PURIFY_DO_LOGGING)
  set(PURIFY_TEST_LOG_LEVEL off)
endif()

