# Installs dtschump/CImg into build directory
#
# - GIT_REPOSITORY: defaults to https://github.com/dtschump/CImg.git
# - GIT_TAG: defaults to master
if(CImg_ARGUMENTS)
  cmake_parse_arguments(CImg "" "GIT_REPOSITORY;GIT_TAG;" "" ${CImg_ARGUMENTS})
endif()
if(NOT CImg_GIT_REPOSITORY)
  set(CImg_GIT_REPOSITORY https://github.com/dtschump/CImg.git)
endif()
if(NOT CImg_GIT_TAG)
  set(CImg_GIT_TAG master)
endif()
ExternalProject_Add(
  Lookup-CImg
  PREFIX ${EXTERNAL_ROOT}
  GIT_REPOSITORY ${CImg_GIT_REPOSITORY}
  GIT_TAG ${CImg_GIT_TAG}
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  UPDATE_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
  ${EXTERNAL_ROOT}/src/Lookup-CImg/CImg.h ${EXTERNAL_ROOT}/include/CImg.h
  LOG_DOWNLOAD ON
  )
set(CImg_DIR ${EXTERNAL_ROOT} CACHE PATH "Directory with include/CImg.h" FORCE)
set(CImg_INCLUDE_DIR ${EXTERNAL_ROOT}/include CACHE PATH "Directory with CImg.h")
set(CImg_FOUND TRUE CACHE BOOL "True if CImg was found." FORCE)
set(CImg_LOOKUP_BUILD TRUE CACHE BOOL "" FORCE)


