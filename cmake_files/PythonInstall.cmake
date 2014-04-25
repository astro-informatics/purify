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
    include(CMakeParseArguments)
    install(${INSTALLPYTHON_UNPARSED_ARGUMENTS} DESTINATION ${directory}
            COMPONENT ${component})
endfunction()
