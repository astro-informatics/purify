# Looks up [Sopt](http://basp-group.github.io/sopt/)
#
# - GIT_REPOSITORY: defaults to https://github.com/astro-informatics/sopt.git
# - GIT_TAG: defaults to master
# - BUILD_TYPE: defaults to Release
#
if(Sopt_ARGUMENTS)
  cmake_parse_arguments(Sopt "" "GIT_REPOSITORY;GIT_TAG;BUILD_TYPE;MPI" "" ${Sopt_ARGUMENTS})
endif()
if(NOT Sopt_GIT_REPOSITORY)
    set(Sopt_GIT_REPOSITORY https://github.com/astro-informatics/sopt.git)
endif()
if(NOT Sopt_GIT_TAG)
    set(Sopt_GIT_TAG development)
endif()
if(NOT Sopt_BUILD_TYPE)
    set(Sopt_BUILD_TYPE Release)
endif()
if(NOT Sopt_MPI)
  set(Sopt_MPI OFF)
endif()

# write subset of variables to cache for sopt to use
include(PassonVariables)
passon_variables(Lookup-Sopt
  FILENAME "${EXTERNAL_ROOT}/src/SoptVariables.cmake"
  PATTERNS
      "CMAKE_[^_]*_R?PATH" "CMAKE_C_.*"
      "BLAS_.*" "FFTW3_.*" "TIFF_.*"
      "GreatCMakeCookOff_DIR"
  ALSOADD
      "\nset(CMAKE_INSTALL_PREFIX \"${EXTERNAL_ROOT}\" CACHE STRING \"\")\n"
)
ExternalProject_Add(
    Lookup-Sopt
    PREFIX ${EXTERNAL_ROOT}
    GIT_REPOSITORY ${Sopt_GIT_REPOSITORY}
    GIT_TAG ${Sopt_GIT_TAG}
    CMAKE_ARGS
      -C "${EXTERNAL_ROOT}/src/SoptVariables.cmake"
      -DBUILD_SHARED_LIBS=OFF
      -DCMAKE_BUILD_TYPE=${Sopt_BUILD_TYPE}
      -Ddompi=${Sopt_MPI}
      -Dtests=OFF
      -Dpython=OFF
      -Dexamples=OFF
      -Dbenchmarks=OFF
      -Dlogging=${logging}
      -DNOEXPORT=TRUE
      -Dopenmp=${openmp}
      -Ddocs=OFF
      -Dcppflow=${cppflow}
    INSTALL_DIR ${EXTERNAL_ROOT}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
add_recursive_cmake_step(Lookup-Sopt DEPENDEES install)

foreach(dep Lookup-Eigen3 Lookup-spdlog)
  lookup_package(${dep})
  if(TARGET ${dep})
    add_dependencies(Lookup-Sopt ${dep})
  endif()
endforeach()
