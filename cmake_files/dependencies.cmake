# Scripts to run purify from build directory. Good for testing/debuggin.
include(EnvironmentScript)
# Look up packages: if not found, installs them
include(PackageLookup)

# Look for external software
find_package(FFTW3 REQUIRED DOUBLE)
find_package(TIFF REQUIRED)
find_package(CBLAS REQUIRED)
set(PURIFY_BLAS_H "${BLAS_INCLUDE_FILENAME}")

# Look up packages: if not found, installs them
lookup_package(Sopt REQUIRED ARGUMENTS GIT_TAG features/cmake)
lookup_package(CFitsIO REQUIRED ARGUMENTS CHECKCASA)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")

if(openmp)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  else()
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()

find_package(Doxygen)

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
