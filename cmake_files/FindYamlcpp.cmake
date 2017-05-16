find_path(Yamlcpp_INCLUDE_DIR NAMES yaml.h PATH_SUFFIXES yaml-cpp)
find_library(Yamlcpp_LIBRARY NAMES yaml-cpp)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  Yamlcpp  DEFAULT_MSG Yamlcpp_INCLUDE_DIR Yamlcpp_LIBRARY)
