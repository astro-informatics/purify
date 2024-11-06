# Looks up [CasaCore](https://github.com/casacore/casacore)
#
# - GIT_REPOSITORY: defaults to https://github.com/casacore/casacore.git
# - GIT_TAG: defaults to master
# - BUILD_TYPE: defaults to Release
#
include(ExternalProject)
if(CasaCore_ARGUMENTS)
  cmake_parse_arguments(CasaCore "" "GIT_REPOSITORY;GIT_TAG;OPENMPI;THREADS" ""
    ${CasaCore_ARGUMENTS})
endif()
if(NOT CasaCore_GIT_REPOSITORY)
    set(CasaCore_GIT_REPOSITORY https://github.com/casacore/casacore.git)
endif()
if(NOT CasaCore_GIT_TAG)
    #set(CasaCore_GIT_TAG v2.4.1) # Hardcoded to latest release
    set(CasaCore_GIT_TAG v3.5.0) # Hardcoded to latest release
endif()
#if(NOT CasaCore_BUILD_TYPE)
#    set(CasaCore_BUILD_TYPE Release)
#endif()
if(NOT CasaCore_OPENMP)
  set(CasaCore_OPENMP OFF)
endif()
if(NOT CasaCore_THREADS)
  set(CasaCore_THREADS OFF)
endif()

ExternalProject_Add(
    Lookup-CasaCore
    GIT_REPOSITORY ${CasaCore_GIT_REPOSITORY}
    GIT_TAG ${CasaCore_GIT_TAG}
    PREFIX ${CMAKE_BINARY_DIR}/external
    CMAKE_ARGS
    -DUSE_OPENMP=${CasaCore_OPENMP}
    -DBUILD_PYTHON=OFF
    -DBUILD_PYTHON3=OFF
    -DUSE_THREADS=${CasaCore_THREADS}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
    -DCMAKE_INSTALL_LIBDIR=${CMAKE_SHARED_LIBRARY_PREFIX}
    #INSTALL_DIR ${EXTERNAL_ROOT}
    UPDATE_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

ExternalProject_Get_Property(Lookup-CasaCore install_dir)
set(CasaCore_DIR "${install_dir}")
set(CasaCore_INCLUDE_DIR "${CasaCore_DIR}/include")
set(CasaCore_LIBRARY_DIR "${CasaCore_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}")
set(CasaCore_LIBRARIES
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_coordinates${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_images${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_msfits${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_lattices${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_mirlib${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_fits${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_ms${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_measures${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_scimath${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_tables${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_scimath_f${CMAKE_SHARED_LIBRARY_SUFFIX}"
    "${CasaCore_LIBRARY_DIR}/${CMAKE_SHARED_MODULE_PREFIX}casa_casa${CMAKE_SHARED_LIBRARY_SUFFIX}"
)
#message(STATUS "libs=${CasaCore_LIBRARIES}")

