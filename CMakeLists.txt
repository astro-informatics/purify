cmake_minimum_required(VERSION 2.8)
project(Purify C CXX)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes
include(LookUp-GreatCMakeCookOff)

option(tests          "Enable testing"                                  on)
option(examples       "Compile examples"                                on)
option(benchmarks     "Enable benchmarking"                             on)
option(data           "Download measurement set for testing"            off)
option(openmp         "Enable OpenMP"                                   on)
option(logging        "Enable logging"                                  on)
option(dompi          "Enable MPI"                                      on)
option(doaf           "Enable ArrayFire"                                off)
option(docimg         "Enable CImg"                                     off)
option(docasa         "Enable CASA"                                     off)
option(docs           "Build documentation"                             on)

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

if(tests AND data)
  include(data_dependencies)
endif()

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
