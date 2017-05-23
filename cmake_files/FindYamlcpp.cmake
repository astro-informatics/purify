find_path(Yamlcpp_INCLUDE_DIR NAMES yaml.h PATH_SUFFIXES yaml-cpp)

set(CMAKE_REQUIRED_INCLUDES ${YAMLCPP_INCLUDE_DIR})
set(CMAKE_REQUIRED_QUIET True)

if(Yamlcpp_STATIC_LIBRARY)
    set(Yamlcpp_STATIC libyaml-cpp.a)
endif()

# find the yaml-cpp library
find_library(Yamlcpp_LIBRARY NAMES ${Yamlcpp_STATIC} yaml-cpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Yamlcpp DEFAULT_MSG Yamlcpp_INCLUDE_DIR Yamlcpp_LIBRARY)
mark_as_advanced(Yamlcpp_INCLUDE_DIR Yamlcpp_LIBRARY)

