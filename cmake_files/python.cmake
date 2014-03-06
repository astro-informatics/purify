find_package(PythonInterp REQUIRED)
# Check python version 
# if(PYTHONINTERP_FOUND AND NOT PYTHON_VERSION)
#   execute_process( 
#     COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(\"%i.%i.%i\" % sys.version_info[:3])"
#     OUTPUT_VARIABLE PYTHON_VERSION
#   )
#   if( PYTHON_VERSION )
#     string (STRIP ${PYTHON_VERSION} PYTHON_VERSION)
#     set(PYTHON_VERSION ${PYTHON_VERSION} CACHE STRING "Version of the Python interpreter.")
#     mark_as_advanced(PYTHON_VERSION)
#     MESSAGE(STATUS "[Python] Version: ${PYTHON_VERSION}")
#   else( PYTHON_VERSION )
#     MESSAGE(STATUS "Could not determine python version.")
#   endif( PYTHON_VERSION )
# endif(PYTHONINTERP_FOUND AND NOT PYTHON_VERSION)

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
  find_path(PYTHON_INCLUDE_DIR 
    NAMES Python.h
    HINTS ${PYTHON_INTERP_PREFIX}
    PATH_SUFFIXES
      include/python${_CURRENT_VERSION}mu
      include/python${_CURRENT_VERSION}m
      include/python${_CURRENT_VERSION}u
      include/python${_CURRENT_VERSION}
    NO_DEFAULT_PATH
  )
  set(PYTHON_INCLUDE_DIR ${PYTHON_INCLUDE_DIR} CACHE PATH "Path to python includes")

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

