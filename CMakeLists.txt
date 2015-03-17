cmake_minimum_required(VERSION 2.8)
project(Purify C)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes
include(LookUp-GreatCMakeCookOff)

# Set version and build id of this package
include(VersionAndGitRef)
set_version(0.1.1)
get_gitref()

# Add C99 flag, since code uses inline
# yeah, yeah it is GNU C, and others since for ever, but it was not part of the standard.
include(AddC99Flags)

option(tests          "Enable testing"                                  on)
option(openmp         "Enable OpenMP"                                   off)
option(python         "Enable python bindings"                          on)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(NOT "$ENV{VIRTUAL_ENV}" STREQUAL "")
    set(CMAKE_INSTALL_PREFIX
      "$ENV{VIRTUAL_ENV}" CACHE PATH "Installation location" FORCE)
    message(STATUS
      "Install dir unspecified and virtual env found. "
      "Defaults to installing in ${CMAKE_INSTALL_PREFIX}"
    )
  endif()
endif()

# Extra parameters
if(NOT DEFINED LIBRARY_INSTALL_PATH)
    set(LIBRARY_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()
include(rpath)

# include exernal dependencies
include(dependencies)
if(EXISTS "${EXTERNAL_ROOT}/lib/")
    install(DIRECTORY "${EXTERNAL_ROOT}/lib/"
        DESTINATION "${LIBRARY_INSTALL_PATH}"
        PATTERN "pkgconfig" EXCLUDE
        PATTERN "*.a" EXCLUDE
    )
endif()
if(python)
    include(python_dependencies)
endif()

if(tests)  # Adds ctest
    enable_testing()
endif()

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(python)
add_subdirectory(doc)

if(LINK_TO_ABSOLUTE_PYTHON_PATH AND APPLE)
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake_files/AbsolutePythonPath.in.cmake"
        "${PROJECT_BINARY_DIR}/post_install/AbsolutePythonPath.cmake"
        @ONLY
    )
    file(WRITE
        "${PROJECT_BINARY_DIR}/post_install/CMakeLists.txt"
        "install(SCRIPT \"AbsolutePythonPath.cmake\")"
    )
    add_subdirectory(
        "${PROJECT_BINARY_DIR}/post_install" # Generated source dir
        "${PROJECT_BINARY_DIR}/post_install/build" # Build dir
    )
endif()
