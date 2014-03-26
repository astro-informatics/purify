# Finds python interpreter and library.
# Tries to make sure one fits the other
# Finds default PYTHON_PKG_DIR, as given by distutils
# Creates function to install relative to PYTHON_PKG_DIR
find_package(PythonInterp REQUIRED)

# Find python package directory
if(NOT DEFINED PYTHON_PKG_DIR)
  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c
              "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
              OUTPUT_VARIABLE PYTHON_PKG_DIR
  )
  if(PYTHON_PKG_DIR )
    string (STRIP ${PYTHON_PKG_DIR} PYTHON_PKG_DIR)
    set(PYTHON_PKG_DIR ${PYTHON_PKG_DIR} CACHE PATH "Main python package repository.")
    mark_as_advanced(PYTHON_PKG_DIR)
  endif(PYTHON_PKG_DIR)
endif(NOT DEFINED PYTHON_PKG_DIR)

execute_process(
  COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.prefix)"
  OUTPUT_VARIABLE PYTHON_INTERP_PREFIX
)
string(STRIP "${PYTHON_INTERP_PREFIX}" PYTHON_INTERP_PREFIX)


# Tries to force CMake to look in appropriate place.
set(OLD_CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH})
list(INSERT CMAKE_PREFIX_PATH 0 ${PYTHON_INTERP_PREFIX})

# Now look for libraries
find_package(PythonLibs REQUIRED)
# But it is likely headers are from different place under macos + brew
if(NOT ${PYTHONLIBS_VERSION_STRING} VERSION_EQUAL ${PYTHON_VERSION_STRING})
  unset(PYTHON_INCLUDE_DIR)
  string(REGEX REPLACE "([0-9]\\.[0-9]+)\\.[0-9]+" "\\1" version_string "${PYTHON_VERSION_STRING}")
  find_path(_PYTHON_INCLUDE_DIR
    NAMES Python.h
    HINTS ${PYTHON_INTERP_PREFIX}/
    PATH_SUFFIXES
      include/python${version_string}mu
      include/python${version_string}m
      include/python${version_string}u
      include/python${version_string}
    NO_DEFAULT_PATH
  )
  set(PYTHON_INCLUDE_DIR ${_PYTHON_INCLUDE_DIR} CACHE PATH "Path to python includes" FORCE)

  if(PYTHON_INCLUDE_DIR AND EXISTS "${PYTHON_INCLUDE_DIR}/patchlevel.h")
    file(STRINGS
      "${PYTHON_INCLUDE_DIR}/patchlevel.h"
      PYTHONLIBS_VERSION_STRING
      REGEX "^#define[ \t]+PY_VERSION[ \t]+\"[^\"]+\""
    )
    string(REGEX REPLACE
      "^#define[ \t]+PY_VERSION[ \t]+\"([^\"]+)\".*" "\\1"
      PYTHONLIBS_VERSION_STRING
      "${PYTHONLIBS_VERSION_STRING}"
    )
    message(STATUS "[PythonLibs] corrected path and version to ${PYTHONLIBS_VERSION_STRING}" )
  endif()
endif()


find_package(Numpy REQUIRED)

set(CMAKE_PREFIX_PATH ${OLD_CMAKE_PREFIX_PATH})

include(CMakeParseArguments)
# Installs relative to PYTHON_PKG_DIR
function(install_python)
    cmake_parse_arguments(INSTALLPYTHON "" "DESTINATION;COMPONENT" "" ${ARGN})
    if(INSTALLPYTHON_DESTINATION AND IS_ABSOLUTE ${INSTALLPYTHON_DESTINATION})
        set(directory ${INSTALLPYTHON_DESTINATION})
    else()
        set(directory ${PYTHON_PKG_DIR})
        if(INSTALLPYTHON_DESTINATION)
            set(directory ${directory}/${INSTALLPYTHON_DESTINATION})
        endif()
    endif()
    set(component python)
    if(INSTALLPYTHON_COMPONENT)
        set(component ${INSTALLPYTHON_COMPONENT})
    endif()
    install(${INSTALLPYTHON_UNPARSED_ARGUMENTS} DESTINATION ${directory}
            COMPONENT ${component})
endfunction()
