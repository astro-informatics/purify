#include(PackageLookup)  # check for existence, or install external projects
# Scripts to run purify from build directory. Good for testing/debugging.
include(EnvironmentScript)
# Look up packages: if not found, installs them

# On different platforms the CMakeDeps generator in conan seems to install eigen
# as either "eigen" or "Eigen3" because the recipe does not explicitly define the
# name (yet). To work around this we have to check for both.
find_package(eigen NAMES Eigen3)
if(eigen_FOUND OR Eigen3_FOUND)
  if(eigen_INCLUDE_DIR)
    set(EIGEN3_INCLUDE_DIR ${eigen_INCLUDE_DIR} CACHE INTERNAL "")
  elseif(Eigen3_INCLUDE_DIR)
    set(EIGEN3_INCLUDE_DIR ${Eigen3_INCLUDE_DIR} CACHE INTERNAL "")
  endif()
else()
  message(FATAL_ERROR "Eigen is required")
endif()

find_package(CFitsIO REQUIRED)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30.0")
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost COMPONENTS system filesystem REQUIRED)

find_package(yaml-cpp REQUIRED)

if (onnxrt)
  find_package(onnxruntime REQUIRED)
endif()

find_package(sopt REQUIRED)
set(PURIFY_ONNXRT FALSE)
if (onnxrt)
  if (${sopt_HAS_ORT})
    set(PURIFY_ONNXRT TRUE)
  else()
    message(FATAL_ERROR "SOPT built without ONNXrt support")
  endif()
endif()

find_package(Cubature QUIET)
if(NOT Cubature_FOUND)
  message(STATUS "Cubature not found. Attempt to install...")
  include(LookUpCubature)
  set(PURIFY_CUBATURE_LOOKUP TRUE)
endif()

# Look for external software
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads)
  if(THREADS_FOUND)
    add_compile_options(-pthread)
  endif(THREADS_FOUND)
endif()

if(docs)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.3.0")
    cmake_policy(SET CMP0057 NEW)
  endif()
  find_package(Doxygen REQUIRED dot)
endif()

if(tests)  # Adds ctest
  enable_testing()
  find_package(Catch2)
  include(AddCatchTest)
endif()

if(examples)
  find_package(TIFF REQUIRED)
endif()

if(tests OR examples)
  file(COPY data DESTINATION .)
endif()

if(benchmarks)
  find_package(benchmark REQUIRED)
  #include(AddBenchmark)
endif()

# Always find open-mp, since it may be used by sopt
if (openmp)
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.12.0")
    cmake_policy(SET CMP0074 NEW)
  endif()
  find_package(OpenMP)
  if (OPENMP_FOUND)
    # Set PURIFY_OPENMP to TRUE when OpenMP is both found and requested
    set(PURIFY_OPENMP TRUE)
  else()
    # Set to FALSE when OpenMP is not found or not requested
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()

find_package(fftw3 NAMES FFTW3 REQUIRED)

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
  find_package(CasaCore OPTIONAL_COMPONENTS ms QUIET)
  if(NOT CasaCore_FOUND)
    message(STATUS "Casacore requested but not found. Attempt to install...")
    include(LookUpCasaCore)
    set(PURIFY_CASACORE_LOOKUP TRUE)
  endif()
  set(PURIFY_CASACORE TRUE)
endif()

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
