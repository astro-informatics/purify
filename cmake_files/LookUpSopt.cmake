# Looks up [Sopt](http://basp-group.github.io/sopt/)
#
# - GIT_REPOSITORY: defaults to https://github.com/UCL/sopt.git
# - GIT_TAG: defaults to master
# - BUILD_TYPE: defaults to Release
#
if(Sopt_ARGUMENTS)
    cmake_parse_arguments(Sopt "" "GIT_REPOSITORY;GIT_TAG;BUILD_TYPE" ""
        ${Sopt_ARGUMENTS})
endif()
if(NOT Sopt_GIT_REPOSITORY)
    set(Sopt_GIT_REPOSITORY https://github.com/UCL/sopt.git)
endif()
if(NOT Sopt_GIT_TAG)
    set(Sopt_GIT_TAG master)
endif()
if(NOT Sopt_BUILD_TYPE)
    set(Sopt_BUILD_TYPE Release)
endif()
# Read all blas related input
include(CachedVariables)
cached_variables(blaslibs "BLAS_.*")
cached_variables(fftwlibs "FFTW3_.*")
cached_variables(tifflibs "TIFF_.*")
ExternalProject_Add(
    Sopt
    PREFIX ${EXTERNAL_ROOT}
    GIT_REPOSITORY ${Sopt_GIT_REPOSITORY}
    GIT_TAG ${Sopt_GIT_TAG}
    CMAKE_ARGS
      -DBUILD_SHARED_LIBS=OFF
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
      -DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}
      -DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}
      -DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}
      -DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}
      -DCMAKE_BUILD_TYPE=${Sopt_BUILD_TYPE}
      -DCMAKE_INSTALL_PREFIX=${EXTERNAL_ROOT}
      -DBLAS_INCLUDE_DIR=${BLAS_INCLUDE_DIR}
      -DNOEXPORT=TRUE
      ${blaslibs}
      ${fftwlibs}
      ${tifflibs}
    INSTALL_DIR ${EXTERNAL_ROOT}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
add_recursive_cmake_step(Sopt DEPENDEES install)
