cmake_minimum_required(VERSION 2.8)
project(Purify C)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Set version and build id of this package
include(version_and_build_id)
version_and_build_id(0.1)

# Add C99 flag, since code uses inline 
# yeah, yeah it is GNU C, and others since for ever, but it was not part of the standard. 
include(c99_flags)

option(tests          "Enable testing"                         on)
option(openmp         "Enable OpenMP"                          on)
option(python         "Enable python bindings"                 on)


# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes 
include(LookUp-GreatCMakeCookOff)

# include exernal dependencies
include(dependencies)
if(tests)
  enable_testing()
endif()

if(python) 
  include(python)
endif()

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(python)
