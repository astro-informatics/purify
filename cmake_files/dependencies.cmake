# Look for external software
find_package(FFTW3 REQUIRED DOUBLE)
find_package(TIFF REQUIRED)
find_package(BLAS REQUIRED)
# find_package blas does not look for cblas.h
if(NOT BLAS_INCLUDE_DIR)
  find_path(BLAS_INCLUDE_DIR cblas.h)
endif()

# Find somewhat extenal package
include(PackageLookup)
lookup_package(Sopt REQUIRED ARGUMENTS GIT_TAG features/cmake)
lookup_package(CFitsIO REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")

if(openmp)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  else()
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()

if(python)
    pip_install(cython LOCAL REQUIRED)
    if(tests)
        pip_install(nose LOCAL REQUIRED)
    endif()
    pip_install(numpy REQUIRED)
    pip_install(scipy REQUIRED)

    # Finds additional info, like libraries, include dirs...
    find_package(Numpy REQUIRED)
endif()

