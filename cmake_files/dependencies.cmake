#include(PackageLookup)  # check for existence, or install external projects
# Scripts to run purify from build directory. Good for testing/debugging.
include(EnvironmentScript)
# Look up packages: if not found, installs them

# On different platforms the CMakeDeps generator in conan seems to install eigen
# as either "eigen" or "Eigen3" because the recipe does not explicitly define the
# name (yet). To work around this we have to check for both.
find_package(eigen NAMES Eigen3 REQUIRED)
if(eigen_INCLUDE_DIR)
  set(EIGEN3_INCLUDE_DIR ${eigen_INCLUDE_DIR} CACHE INTERNAL "")
elseif(Eigen3_INCLUDE_DIR)
  set(EIGEN3_INCLUDE_DIR ${Eigen3_INCLUDE_DIR} CACHE INTERNAL "")
endif()

find_package(CFitsIO MODULE REQUIRED)

find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(yaml-cpp REQUIRED)

find_package(Sopt REQUIRED)

find_package(Cubature QUIET)
if(NOT Cubature_FOUND)
  message(STATUS "Cubature not found. Attempt to install...")
  include(LookUpCubature)
endif()

# Look for external software
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads)
  if(THREADS_FOUND)
    add_compile_options(-pthread)
  endif(THREADS_FOUND)
endif()

if(logging)
  find_package(spdlog REQUIRED)
endif()

if(docs)
  cmake_policy(SET CMP0057 NEW)
  find_package(doxygen REQUIRED dot)
  if(NOT doxygen_FOUND)
    mesage(FATAL_ERROR "Could not find Doxygen or dot")
  endif()
endif()

if(examples)
  find_package(TIFF REQUIRED)
endif()


if(cppflow)
  find_package(cppflow)
  find_library(TENSORFLOW_LIB tensorflow REQUIRED)
endif()

# Always find open-mp, since it may be used by sopt
find_package(OpenMP)
if(openmp AND NOT OPENMP_FOUND)
  message(STATUS "Could not find OpenMP. Compiling without.")
endif()
set(PURIFY_OPENMP_FFTW FALSE)
if(openmp AND OPENMP_FOUND)
  # Set PURIFY_OPENMP to TRUE when OpenMP is both found and requested
  set(PURIFY_OPENMP TRUE)

  # Add the OpenMP Library
  add_library(openmp::openmp INTERFACE IMPORTED GLOBAL)

  # Set compiler and linker options to the defaults for CXX
  # TODO: Should this be done automatically?
  #       Check when we update CMake and the OpenMP linking to
  #       https://cliutils.gitlab.io/modern-cmake/chapters/packages/OpenMP.html
  #       possibly using
  #       https://cmake.org/cmake/help/latest/module/FindOpenMP.html
  set_target_properties(openmp::openmp PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
    INTERFACE_LINK_LIBRARIES  "${OpenMP_CXX_FLAGS}")
else()
  # Set to FALSE when OpenMP is not found or not requested
  set(PURIFY_OPENMP FALSE)
endif()

find_package(FFTW3 REQUIRED)

set(PURIFY_MPI FALSE)
if(dompi)
  find_package(MPI REQUIRED)
  set(PURIFY_MPI TRUE)
endif()
find_package(TIFF REQUIRED)

set(PURIFY_ARRAYFIRE FALSE)
if(doaf)
  lookup_package(ArrayFire REQUIRED)
  set(PURIFY_ARRAYFIRE TRUE)
endif()

set(PURIFY_CImg FALSE)
if(docimg)
  find_package(X11)
  find_package(cimg)
  set(PURIFY_CImg TRUE)
endif()

set(PURIFY_CASACORE FALSE)
if(docasa)
  find_package(CasaCore OPTIONAL_COMPONENTS ms)
  if(NOT CasaCore_FOUND)
    message(FATAL_ERROR "CasaCore was requested but could not be found")
  endif()
  set(PURIFY_CASACORE TRUE)
endif()

set(PURIFY_CPPFLOW FALSE)
if(cppflow)
  find_package(cppflow)
  find_library(TENSORFLOW_LIB tensorflow REQUIRED)
  set(PURIFY_CPPFLOW TRUE)
endif()

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
