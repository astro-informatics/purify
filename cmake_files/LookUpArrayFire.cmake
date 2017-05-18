# Looks up [ArrayFire](git@github.com:arrayfire/arrayfire.git)
#
# - GIT_REPOSITORY: defaults to git@github.com:arrayfire/arrayfire.git
# - GIT_TAG: defaults to master
# - BUILD_TYPE: defaults to Release
#
if(ArrayFire_ARGUMENTS)
  cmake_parse_arguments(ArrayFire "" "GIT_REPOSITORY;GIT_TAG;BUILD_TYPE;CUDA;OPENCL" "" ${ArrayFire_ARGUMENTS})
endif()
if(NOT ArrayFire_GIT_REPOSITORY)
  set(ArrayFire_GIT_REPOSITORY git@github.com:arrayfire/arrayfire.git)
endif()
if(NOT ArrayFire_GIT_TAG)
  set(ArrayFire_GIT_TAG master)
endif()
if(NOT ArrayFire_BUILD_TYPE)
  set(ArrayFire_BUILD_TYPE Release)
endif()
if(NOT ArrayFire_CUDA)
  set(ArrayFire_CUDA OFF)
endif()
if(NOT ArrayFire_OPENCL)
  set(ArrayFire_OPENCL ON)
endif()
ExternalProject_Add(
  Lookup-ArrayFire
  PREFIX ${EXTERNAL_ROOT}
  GIT_REPOSITORY ${ArrayFire_GIT_REPOSITORY}
  GIT_TAG ${ArrayFire_GIT_TAG}
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${EXTERNAL_ROOT}
  -DBUILD_CUDA=${ArrayFire_CUDA}
  -DBUILD_OPENCL=${ArrayFire_OPENCL}
  -DBUILD_TEST=OFF
  -DBUILD_EXAMPLES=OFF
  -DCMAKE_BUILD_TYPE=${ArrayFire_BUILD_TYPE}
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  )
add_recursive_cmake_step(Lookup-ArrayFire DEPENDEES install)
