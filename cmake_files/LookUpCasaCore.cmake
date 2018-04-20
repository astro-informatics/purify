# Looks up [CasaCore](https://github.com/casacore/casacore)
#
# - GIT_REPOSITORY: defaults to https://github.com/casacore/casacore.git
# - GIT_TAG: defaults to master
# - BUILD_TYPE: defaults to Release
#
if(CasaCore_ARGUMENTS)
  cmake_parse_arguments(CasaCore "" "GIT_REPOSITORY;GIT_TAG;BUILD_TYPE;OPENMPI;THREADS" "" ${CasaCore_ARGUMENTS})
endif()
if(NOT CasaCore_GIT_REPOSITORY)
    set(CasaCore_GIT_REPOSITORY https://github.com/casacore/casacore.git)
endif()
if(NOT CasaCore_GIT_TAG)
    set(CasaCore_GIT_TAG v2.4.1) # Hardcoded to latest release
endif()
if(NOT CasaCore_BUILD_TYPE)
    set(CasaCore_BUILD_TYPE Release)
endif()
if(NOT CasaCore_OPENMP)
  set(CasaCore_OPENMP OFF)
endif()
if(NOT CasaCore_THREADS)
  set(CasaCore_THREADS OFF)
endif()

# write subset of variables to cache for casacore to use
include(PassonVariables)
passon_variables(Lookup-CasaCore
  FILENAME "${EXTERNAL_ROOT}/src/CasaCoreVariables.cmake"
  PATTERNS
      "CMAKE_[^_]*_R?PATH" "CMAKE_C_.*"
      "BLAS_.*" "FFTW3_.*" "TIFF_.*"
      "GreatCMakeCookOff_DIR"
  ALSOADD
      "\nset(CMAKE_INSTALL_PREFIX \"${EXTERNAL_ROOT}\" CACHE STRING \"\")\n"
)
ExternalProject_Add(
    Lookup-CasaCore
    PREFIX ${EXTERNAL_ROOT}
    GIT_REPOSITORY ${CasaCore_GIT_REPOSITORY}
    GIT_TAG ${CasaCore_GIT_TAG}
    CMAKE_ARGS
      -C "${EXTERNAL_ROOT}/src/CasaCoreVariables.cmake"
      -DCMAKE_BUILD_TYPE=${CasaCore_BUILD_TYPE}
      -DUSE_OPENMP=${CasaCore_OPENMP}
      -DBUILD_PYTHON=OFF
      -DUSE_THREADS=${CasaCore_THREADS}
    INSTALL_DIR ${EXTERNAL_ROOT}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

set(CasaCore_DIR ${EXTERNAL_ROOT})
add_recursive_cmake_step(Lookup-CasaCore DEPENDEES install)

