
find_path(Cubature_INCLUDE_DIR NAMES cubature.h PATH_SUFFIXES cubature)
find_library(Cubature_LIBRARIES NAMES cubature)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  Cubature  DEFAULT_MSG Cubature_INCLUDE_DIR Cubature_LIBRARIES)
if(NOT Cubature_FOUND)
set(Cubature_INCLUDE_DIR "")
set(Cubature_LIBRARIES "")
endif()
