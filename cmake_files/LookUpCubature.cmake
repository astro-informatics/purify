# Installs stevengj/Cubature into build directory
#
# - GIT_REPOSITORY: defaults to origin stevengj repo on github
# - GIT_TAG: defaults to master
include(ExternalProject)
if(Cubature_ARGUMENTS)
  cmake_parse_arguments(Cubature "" "GIT_REPOSITORY;GIT_TAG" ""
    ${Cubature_ARGUMENTS})
endif()
if(NOT Cubature_GIT_REPOSITORY)
  set(Cubature_GIT_REPOSITORY https://github.com/stevengj/cubature)
endif()
if(NOT Cubature_GIT_TAG)
  set(Cubature_GIT_TAG master)
endif()

ExternalProject_Add(
  Lookup-Cubature
  GIT_REPOSITORY ${Cubature_GIT_REPOSITORY}
  GIT_TAG ${Cubature_GIT_TAG}
  PREFIX "${CMAKE_BINARY_DIR}/external"
  #INSTALL_DIR ${EXTERNAL_ROOT}
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
  -DCMAKE_INSTALL_LIBDIR=${CMAKE_SHARED_LIBRARY_PREFIX}
  # Wrap download, configure and build steps in a script to log output
  UPDATE_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
)
ExternalProject_Get_Property(Lookup-Cubature install_dir)
set(Cubature_DIR "${install_dir}")
set(Cubature_INCLUDE_DIR "${Cubature_DIR}/include")
set(Cubature_LIBRARY_DIR "${Cubature_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}")
set(Cubature_LIBRARIES "cubature")

