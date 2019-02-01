# Installs stevengj/Cubature into build directory
#
# - GIT_REPOSITORY: defaults to origin stevengj repo on github
# - GIT_TAG: defaults to master
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
  PREFIX "${EXTERNAL_ROOT}"
  #INSTALL_DIR ${EXTERNAL_ROOT}
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${EXTERNAL_ROOT}
  # Wrap download, configure and build steps in a script to log output
  UPDATE_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON
  LOG_INSTALL ON
  )
set(Cubature_DIR ${EXTERNAL_ROOT})

add_recursive_cmake_step(Lookup-Cubature DEPENDEES install)
