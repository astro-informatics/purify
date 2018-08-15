# Installs jbeder/yaml-cpp into build directory
#
# - GIT_REPOSITORY: defaults to origin jbeder repo on github
# - GIT_TAG: defaults to master
if(Yamlcpp_ARGUMENTS)
  cmake_parse_arguments(Yamlcpp "" "GIT_REPOSITORY;GIT_TAG" ""
    ${Yamlcpp_ARGUMENTS})
endif()
if(NOT Yamlcpp_GIT_REPOSITORY)
  set(Yamlcpp_GIT_REPOSITORY https://github.com/jbeder/yaml-cpp)
endif()
if(NOT Yamlcpp_GIT_TAG)
  set(Yamlcpp_GIT_TAG master)
endif()
ExternalProject_Add(
    Lookup-Yamlcpp
    GIT_REPOSITORY ${Yamlcpp_GIT_REPOSITORY}
    GIT_TAG ${Yamlcpp_GIT_TAG}
    PREFIX "${EXTERNAL_ROOT}"
    # INSTALL_DIR ${EXTERNAL_ROOT}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${EXTERNAL_ROOT}
      -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC
    # Wrap download, configure and build steps in a script to log output
    UPDATE_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)

add_recursive_cmake_step(Lookup-Yamlcpp DEPENDEES install)
