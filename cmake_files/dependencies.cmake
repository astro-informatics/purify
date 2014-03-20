# Look for external software
find_package(FFTW3 REQUIRED)
find_package(TIFF REQUIRED)
find_package(CFitsIO REQUIRED)
find_package(BLAS REQUIRED)
# find_package blas does not look for cblas.h
if(NOT EXISTS BLAS_INCLUDE_DIR)
  find_path(BLAS_INCLUDE_DIR cblas.h)
endif()

# Find somewhat extenal package
if(EXISTS ${PROJECT_SOURCE_DIR}/../sopt)
  list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/../sopt)
endif()
find_package(SOPT)

# Adds include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${BLAS_INCLUDE_DIR}
  ${FFTW3_INCLUDE_DIR}
  ${CFitsIO_INCLUDE_DIR}
  ${TIFF_INCLUDE_DIR}
  ${SOPT_INCLUDE_DIR}
)


# Target libraries for the different executables
set(TARGET_LIBRARIES
     libpurify
     ${SOPT_LIBRARY}
     ${FFTW3_LIBRARY_DOUBLE} 
     ${CFitsIO_LIBRARY} 
     ${TIFF_LIBRARY} 
     ${BLAS_LIBRARIES}
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")

if(openmp)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  else()
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()
