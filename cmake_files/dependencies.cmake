# Scripts to run purify from build directory. Good for testing/debuggin.
include(EnvironmentScript)
# Look up packages: if not found, installs them
include(PackageLookup)
# Get the yaml reader
lookup_package(Yamlcpp REQUIRED  ARGUMENTS GIT_TAG "yaml-cpp-0.6.2")

if(docs)
  cmake_policy(SET CMP0057 NEW)
  find_package(Doxygen REQUIRED dot)
  if(NOT DOXYGEN_FOUND)
    mesage(STATUS "Could not find Doxygen or dot. No building documentation")
    set(docs OFF)
  endif()
endif()

# Look for external software
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads)
  if(THREADS_FOUND)
    add_compile_options(-pthread)
  endif(THREADS_FOUND)
endif()

# Always find open-mp, since it may be used by sopt
find_package(OpenMP)
if(OPENMP_FOUND AND NOT TARGET openmp::openmp)
  add_library(openmp::openmp INTERFACE IMPORTED GLOBAL)
  set_target_properties(openmp::openmp PROPERTIES
    INTERFACE_COMPILE_OPTIONS "-pthread"
    INTERFACE_LINK_LIBRARIES  "${CMAKE_THREAD_LIBS_INIT}")
endif()
if(openmp AND NOT OPENMP_FOUND)
  message(STATUS "Could not find OpenMP. Compiling without.")
endif()
set(PURIFY_OPENMP_FFTW FALSE)
if(openmp AND OPENMP_FOUND)
  set_target_properties(openmp::openmp PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
    INTERFACE_LINK_LIBRARIES  "${OpenMP_CXX_FLAGS}")

  set(PURIFY_OPENMP TRUE)
  find_package(FFTW3 REQUIRED DOUBLE SERIAL COMPONENTS OPENMP)
  set(FFTW3_DOUBLE_LIBRARY fftw3::double::serial)
  if(TARGET fftw3::double::openmp)
    list(APPEND FFTW3_DOUBLE_LIBRARY fftw3::double::openmp)
    set(PURIFY_OPENMP_FFTW TRUE)
  endif()
else()
  set(PURIFY_OPENMP FALSE)
  find_package(FFTW3 REQUIRED DOUBLE)
  set(FFTW3_DOUBLE_LIBRARY fftw3::double::serial)
endif()

set(PURIFY_MPI FALSE)
if(dompi)
  find_package(MPI REQUIRED)
  set(PURIFY_MPI TRUE)
endif()
find_package(TIFF REQUIRED)


lookup_package(Boost REQUIRED COMPONENTS filesystem)

lookup_package(Eigen3 REQUIRED ARGUMENTS MD5 9e30f67e8531477de4117506fe44669b URL https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz)
if(NOT EIGEN3_FOUND)
  set(EIGEN3_INCLUDE_DIR "")
endif()

set(PURIFY_ARRAYFIRE FALSE)
if(doaf)
  lookup_package(ArrayFire REQUIRED)
  set(PURIFY_ARRAYFIRE TRUE)
endif()

if(logging)
  lookup_package(spdlog REQUIRED)
endif()
# Look up packages: if not found, installs them
# Unless otherwise specified, if purify is not on master, then sopt will be
# downloaded from development branch.
if(NOT Sopt_GIT_TAG)
  set(Sopt_GIT_TAG development CACHE STRING "Branch/tag when downloading sopt")
endif()
if(NOT Sopt_GIT_REPOSITORY)
  set(Sopt_GIT_REPOSITORY https://www.github.com/astro-informatics/sopt.git
    CACHE STRING "Location when downloading sopt")
endif()
if(dompi)
  lookup_package(Sopt REQUIRED COMPONENTS mpi ARGUMENTS
    GIT_REPOSITORY ${Sopt_GIT_REPOSITORY}
    GIT_TAG ${Sopt_GIT_TAG}
    MPI "TRUE")
else()
  lookup_package(Sopt REQUIRED ARGUMENTS
    GIT_REPOSITORY ${Sopt_GIT_REPOSITORY}
    GIT_TAG ${Sopt_GIT_TAG})
endif()

lookup_package(CFitsIO REQUIRED)
if(docimg)
  set(PURIFY_CImg TRUE)
  find_package(X11)
  lookup_package(CImg REQUIRED)
endif()
if(docasa)
  find_package(CasaCore OPTIONAL_COMPONENTS ms)
  if(NOT CasaCore_FOUND)
    lookup_package(CasaCore REQUIRED)
  endif()
  set(PURIFY_CASACORE TRUE)
endif()

lookup_package(Cubature REQUIRED)

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
