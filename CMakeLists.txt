cmake_minimum_required(VERSION 3.5.1)
project(Purify C CXX)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build)

option(tests          "Enable testing"                                  on)
option(examples       "Compile examples"                                off)
option(benchmarks     "Enable benchmarking"                             off)
option(openmp         "Enable OpenMP"                                   on)
option(dompi          "Enable MPI"                                      on)
option(doaf           "Enable ArrayFire"                                off)
option(docimg         "Enable CImg"                                     off)
option(docasa         "Enable CASA"                                     off)
option(docs           "Build documentation"                             off)
option(coverage       "Build coverage"                                  off)
option(onnxrt         "Build with ONNXruntime interface"                off)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build Type" FORCE)
endif()
message(STATUS "Building purify in ${CMAKE_BUILD_TYPE} mode")

# Set version and build id of this package
include(version)
set(Sopt_GIT_TAG "development" CACHE STRING "Branch/tag when downloading sopt")

## we are doing c++11
#include(AddCPP11Flags)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# sets up rpath so libraries can be found
include(rpath)

# include exernal dependencies
include(dependencies)

set(PURIFY_TEST_LOG_LEVEL critical CACHE STRING "Level when logging tests")
set_property(CACHE PURIFY_TEST_LOG_LEVEL
             PROPERTY STRINGS off critical error warn info debug trace)

# If PURIFY_OPENMP is set to False in dependencies, link libpthread here so that the linker finds it
# Following advice from https://stackoverflow.com/questions/1620918/cmake-and-libpthread
if(NOT PURIFY_OPENMP)
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")

if(tests AND coverage)
  # Enable code coverage.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
  # Build with debugging information to make the output meaningful.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  # Disable optimizations to get the most accurate results.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()

add_subdirectory(cpp)


# Exports Purify so other packages can access it
include(export_purify)
