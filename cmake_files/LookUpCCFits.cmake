# Looks up CCFits
#
set(CCFits_URL http://heasarc.gsfc.nasa.gov/fitsio/CCfits/CCfits-2.4.tar.gz)
set(CCFits_SHA dd9a11d964590dd5f3fd5a3491b2c1ce)

find_program(PATCH_EXECUTABLE patch)
include(PassonVariables)
passon_variables(CCfits
  FILENAME "${EXTERNAL_ROOT}/src/CCFitsVariables.cmake"
  PATTERNS
      "CMAKE_[^_]*_R?PATH" "CMAKE_C_.*"
      "BLAS_.*" "FFTW3_.*" "TIFF_.*"
  ALSOADD
      "\nset(CMAKE_INSTALL_PREFIX \"${EXTERNAL_ROOT}\" CACHE STRING \"\")\n"
)
if(NOT TARGET CFitsIO)
  find_package(CFitsIO REQUIRED)
  get_filename_component(cfitsio_libdir "${CFitsIO_LIBRARY}" DIRECTORY)
  set(cfitsio_include_dir "${CFitsIO_INCLUDE_DIR}")
else()
  get_filename_component(cfitsio_libdir "${EXTERNAL_ROOT}/lib" DIRECTORY)
  set(cfitsio_include_dir "${EXTERNAL_ROOT}/include")
endif()
ExternalProject_Add(
    Lookup-CCFits
    PREFIX ${EXTERNAL_ROOT}
    URL ${CCFits_URL}
    URL_MD5 ${CCFist_SHA}
    SOURCE_DIR ${EXTERNAL_ROOT}/src/CCfits
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND
    ./configure --prefix=${EXTERNAL_ROOT}
         --with-cfitsio-include=${cfitsio_include_dir}
         --with-cfitsio-libdir=${cfitsio_libdir}
    PATCH_COMMAND ${PATCH_EXECUTABLE} < ${PROJECT_SOURCE_DIR}/cmake_files/ccfits.patch
    INSTALL_DIR ${EXTERNAL_ROOT}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
if(TARGET CFitsIO)
  add_dependencies(Lookup-CCFits CFitsIO)
endif()
add_recursive_cmake_step(Lookup-CCFits DEPENDEES install)
set(CCFits_INCLUDE_DIR "")
set(CCFits_LIBRARY "")
