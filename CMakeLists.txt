cmake_minimum_required(VERSION 2.8)
project(Purify C)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes
include(LookUp-GreatCMakeCookOff)

# Set version and build id of this package
include(VersionAndGitRef)
set_version(0.1)
get_gitref()

# Add C99 flag, since code uses inline
# yeah, yeah it is GNU C, and others since for ever, but it was not part of the standard.
include(AddC99Flags)

option(tests          "Enable testing"                         on)
option(openmp         "Enable OpenMP"                          off)
option(python         "Enable python bindings"                 on)


# Set up python environment
if(python)
  # Python interpreter + libraries
  include(CoherentPython)
  # Ability to install new python packages
  include(PythonPackage)
  # A virtual python environment so we don't pollute the original env
  include(PythonVirtualEnv)
endif()

# include exernal dependencies
include(dependencies)

if(tests)
  enable_testing()
endif()

if(python)
  include(cython)
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(python)
