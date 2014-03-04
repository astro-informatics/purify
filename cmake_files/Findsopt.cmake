# Defines the following variables
#
# - SOPT_FOUND if the library is found
# - SOPT_LIBRARY is the path to the library
# - SOPT_INCLUDE_DIR is the path to the include directory
# - SOPT_VERSION_STRING is the version of the library, if sopt_about could be found.

find_path(
  SOPT_INCLUDE_DIR sopt_l1.h
  PATH_SUFFIXES 
  include sopt/include
  DOC "Path to SOPT include directory"
)
find_library(
  SOPT_LIBRARY sopt
  DOC "Path to SOPT library" 
)

# this is only to try and figure out the SOPT version
find_program(SOPT_ABOUT_PROGRAM sopt_about)
if(NOT "${SOPT_ABOUT}" MATCHES "NOTFOUND")
  execute_process(
    COMMAND ${SOPT_ABOUT_PROGRAM}
    RESULT_VARIABLE SOPT_ABOUT_RESULT
    OUTPUT_VARIABLE SOPT_ABOUT_OUTPUT
    ERROR_QUIET
  )
  if(SOPT_ABOUT_RESULT EQUAL 0)
    string(
      REGEX REPLACE
      ".*Version[ ]*:[ ]*([0-9]+\\.[0-9]+).*" 
      "\\1"
      SOPT_VERSION_STRING 
      "${SOPT_ABOUT_OUTPUT}"
    )
  endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  SOPT
  FOUND_VAR SOPT_FOUND
  REQUIRED_VARS SOPT_LIBRARY SOPT_INCLUDE_DIR
  VERSION_VAR SOPT_VERSION_STRING
)
