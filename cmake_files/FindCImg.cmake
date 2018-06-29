find_path(CImg_INCLUDE_DIR NAMES CImg.h PATH_SUFFIXES CImg)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  CImg  DEFAULT_MSG CImg_INCLUDE_DIR)
if(NOT CImg_FOUND)
  set(CImg_INCLUDE_DIR "")
endif()
