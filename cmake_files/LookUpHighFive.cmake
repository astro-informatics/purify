# Installs BlueBrain/HighFive into build directory
#
# - GIT_REPOSITORY: defaults to origin BlueBrain/HighFive repo on github
# - GIT_TAG: defaults to master
include(ExternalProject)
if(HighFive_ARGUMENTS)
  cmake_parse_arguments(HighFive "" "GIT_REPOSITORY;GIT_TAG" ""
    ${HighFive_ARGUMENTS})
endif()
if(NOT HighFive_GIT_REPOSITORY)
  set(HighFive_GIT_REPOSITORY https://github.com/BlueBrain/HighFive)
endif()
if(NOT HighFive_GIT_TAG)
  set(HighFive_GIT_TAG master)
endif()

set(HighFive_DIR "${CMAKE_INSTALL_PREFIX}/external")
ExternalProject_Add(
  Lookup-HighFive
  GIT_REPOSITORY ${HighFive_GIT_REPOSITORY}
  GIT_TAG ${HighFive_GIT_TAG}
  PREFIX "${CMAKE_BINARY_DIR}/external"
  #INSTALL_DIR ${EXTERNAL_ROOT}
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${HighFive_DIR}
  -DCMAKE_INSTALL_LIBDIR=${CMAKE_SHARED_LIBRARY_PREFIX}
  -DHIGHFIVE_EXAMPLES=OFF
  -DHIGHFIVE_USE_BOOST=OFF
  -DHIGHFIVE_UNIT_TESTS=OFF
  # Wrap download, configure and build steps in a script to log output
  UPDATE_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
)
set(HighFive_INCLUDE_DIR "${HighFive_DIR}/include")
#set(HighFive_LIBRARY_DIR "${HighFive_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}")
#set(HighFive_LIBRARIES "HighFive")

