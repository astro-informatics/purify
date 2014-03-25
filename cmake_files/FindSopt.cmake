# Defines the following variables
#
# - Sopt_FOUND if the library is found
# - Sopt_LIBRARIES is the path to the library
# - Sopt_INCLUDE_DIRS is the path to the include directory
# - Sopt_VERSION_STRING is the version of the library, if sopt_about could be found.

find_path(
  Sopt_INCLUDE_DIRS sopt_l1.h
  PATH_SUFFIXES
  include sopt/include
  DOC "Path to Sopt include directory"
)
find_library(
    Sopt_LIBRARIES sopt
    DOC "Path to Sopt library"
)

# this is only to try and figure out the Sopt version
find_program(Sopt_ABOUT_EXECUTABLE sopt_about)
if(NOT "${Sopt_ABOUT_EXECUTABLE}" MATCHES "NOTFOUND")
  execute_process(
      COMMAND ${Sopt_ABOUT_EXECUTABLE}
      RESULT_VARIABLE Sopt_ABOUT_RESULT
      OUTPUT_VARIABLE Sopt_ABOUT_OUTPUT
    ERROR_QUIET
  )
  if(Sopt_ABOUT_RESULT EQUAL 0)
    string(
      REGEX REPLACE
      ".*Version[ ]*:[ ]*([0-9]+\\.[0-9]+).*"
      "\\1"
      Sopt_VERSION_STRING
      "${Sopt_ABOUT_OUTPUT}"
    )
  endif()
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Sopt
    FOUND_VAR Sopt_FOUND
    REQUIRED_VARS Sopt_LIBRARIES Sopt_INCLUDE_DIRS
    VERSION_VAR Sopt_VERSION_STRING
)
