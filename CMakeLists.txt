cmake_minimum_required(VERSION 2.8)
project(Purify C CXX)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes
include(LookUp-GreatCMakeCookOff)

option(tests          "Enable testing"                                  off)
option(examples       "Compile examples"                                off)
option(benchmarks     "Enable benchmarking"                             off)
option(openmp         "Enable OpenMP"                                   on)
option(logging        "Enable logging"                                  on)
option(dompi          "Enable MPI"                                      on)
option(doaf           "Enable ArrayFire"                                off)
option(docimg         "Enable CImg"                                     off)
option(docasa         "Enable CASA"                                     off)
option(docs           "Build documentation"                             off)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Relese' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build Type" FORCE)
endif()
message(STATUS "Building purify in ${CMAKE_BUILD_TYPE} mode")

# Set version and build id of this package
include(version)

# we are doing c++11
include(AddCPP11Flags)

# sets up rpath so libraries can be found
include(rpath)

# adds logging variables
include(logging)

# include exernal dependencies
include(dependencies)


if(tests)  # Adds ctest
  enable_testing()
  include(AddCatchTest)
endif()

if(benchmarks)
  include(AddBenchmark)
endif()

add_subdirectory(cpp)


# Exports Purify so other packages can access it
include(export_purify)
