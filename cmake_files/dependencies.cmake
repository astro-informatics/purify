include(PackageLookup)  # check for existence, or install external projects
# Scripts to run purify from build directory. Good for testing/debuggin.
include(EnvironmentScript)
# Look up packages: if not found, installs them

# On different platforms the CMakeDeps generator in conan seems to install eigen
# as either "eigen" or "Eigen3" because the recipe does not explicitly define the
# name (yet). To work around this we have to check for both.
find_package(eigen)
find_package(Eigen3)
if(NOT (eigen_FOUND OR Eigen3_FOUND))
  message(FATAL_ERROR "Eigen is required")
else()
  message("eigen_FOUND: ${eigen_FOUND}")
  message("Eigen3_FOUND: ${Eigen3_FOUND}")
endif()

find_package(cfitsio)
if(NOT cfitsio_FOUND)
  message(FATAL_ERROR "CFitsIO is required")
endif()

find_package(Boost COMPONENTS system filesystem REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost is required")
endif()

find_package(yaml-cpp)
if(NOT yaml-cpp_FOUND)
  message(FATAL_ERROR "Yaml-cpp is required")
endif()

find_package(sopt)
if(NOT sopt_FOUND)
  message(FATAL_ERROR "sopt is required")
endif()

lookup_package(Cubature REQUIRED)

# Look for external software
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads)
  if(THREADS_FOUND)
    add_compile_options(-pthread)
  endif(THREADS_FOUND)
endif()

if(logging)
  find_package(spdlog)
  if(NOT spdlog_FOUND)
    message(FATAL_ERROR "logging requires spdlog")
  endif()
endif()

if(docs)
  cmake_policy(SET CMP0057 NEW)
  find_package(doxygen REQUIRED dot)
  if(NOT doxygen_FOUND)
    mesage(FATAL_ERROR "Could not find Doxygen or dot")
  endif()
endif()

if(examples)
  find_package(TIFF)
  if(NOT TIFF_FOUND)
    message(FATAL_ERROR "Examples require TIFF")
  endif()
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

find_package(FFTW3)

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

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
