# Checks and installs python packages
#
# pip_install(
#   PACKAGE             -- name of the package to look for
#   [REQUIRED]
#   [NOINSTALL]         -- only checks package existence. No installation.
#   [LOCAL]             -- makes use of `LOCAL_PYTHON_EXECUTABLE`.
#                          Meant to be used in conjunction with
#                          PythonVirtualEnv recipe
#   [PYTHON_EXECUTABLE <path>]
#                       -- Python executable to use. Incompatible with LOCAL.
#   [ARGUMENTS <list>]  -- Arguments to pass on to pip
# )
#
# This function checks whether a python package exists. If it does, it sets:
#   - `${PACKAGE}_FOUND` to TRUE
#   - `${PACKAGE}_LOCATION` to the directory where the package was found
#   - `${PACKAGE}_PYTHON` to the python executable used during the search
#
# If the package is not found, then the recipe tries to find `pip` and to
# install it via `pip`.
#
# In order to avoid polluting the system or user packages, it is possible to
# use this function in conjunction with a virtual environment created by
# PythonVirtualEnv in the build tree.


# First check for python executable
include(FindPackageHAndleStandardArgs)
find_package(PythonInterp REQUIRED)

include(CMakeParseArguments)

function(_python_executable OUTVAR)
    cmake_parse_arguments(PYEXEC "LOCAL" "PYTHON_EXECUTABLE" "" ${ARGN})
    if(PYEXEC_LOCAL AND PYEXEC_PYTHON_EXECUTABLE)
        message(FATAL_ERROR "Cannot use LOCAL and PYTHON arguments together")
    endif()
    if(PYEXEC_LOCAL AND NOT LOCAL_PYTHON_EXECUTABLE)
        message(FATAL_ERROR "PythonVirtualEnv not included yet.")
    endif()
    if(PYEXEC_LOCAL)
        set(${OUTVAR} ${LOCAL_PYTHON_EXECUTABLE} PARENT_SCOPE)
    elseif(PYEXEC_PYTHON_EXECUTABLE)
        set(${OUTVAR} ${PYEXEC_PYTHON_EXECUTABLE} PARENT_SCOPE)
    else()
        set(${OUTVAR} ${PYTHON_EXECUTABLE} PARENT_SCOPE)
    endif()
    set(${OUTVAR}_UNPARSED_ARGUMENTS ${PYEXEC_UNPARSED_ARGUMENTS} PARENT_SCOPE)
endfunction()

function(_check_package_already_installed PACKAGE OUTVAR)
    cmake_parse_arguments(CHECKPACK "" "WORKING_DIRECTORY" "" ${ARGN})
    _python_executable(LOCALPYTHON ${CHECKPACK_UNPARSED_ARGUMENTS})
    set(wd ${CMAKE_CURRENT_BINARY_DIR})
    if(CHECKPACK_WORKING_DIRECTORY)
        set(wd ${CHECKPACK_WORKING_DIRECTORY})
    endif()

    execute_process(
      COMMAND ${LOCALPYTHON} -c "import ${PACKAGE}"
      WORKING_DIRECTORY ${wd}
      RESULT_VARIABLE PACKAGE_WAS_FOUND
      ERROR_VARIABLE ERROR
      OUTPUT_VARIABLE OUTPUT
    )
    if(PACKAGE_WAS_FOUND EQUAL 0)
        set(${OUTVAR} TRUE PARENT_SCOPE)
    else()
        set(${OUTVAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(pip_install PACKAGE)
    cmake_parse_arguments(PIPINSTALL
        "REQUIRED;NONINSTALL" "PREFIX" "ARGUMENTS" ${ARGN})
    _python_executable(${PACKAGE}_PYTHON ${PIPINSTALL_UNPARSED_ARGUMENTS})
    set(LOCALPYTHON ${${PACKAGE}_PYTHON})
    set(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(ARGUMENTS "")
    if(PIPINSTALL_PREFIX)
        set(prefix ${PIPINSTALL_PREFIX})
        set(WORKING_DIRECTORY ${prefix}/pylibs)
        set(ARGUMENTS
             --install-option=--install-purelib=${prefix}/pylibs
             --install-option=--install-scripts=${prefix}/bin
             --install-option=--prefix=${prefix}
        )
        if(NOT EXISTS ${prefix}/pylibs)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E make_directory ${prefix}/pylibs
                OUTPUT_QUIET
            )
        endif()
    endif()


    _check_package_already_installed(${PACKAGE} package_FOUND
        PYTHON_EXECUTABLE ${LOCALPYTHON}
        WORKING_DIRECTORY ${WORKING_DIRECTORY}
    )
    # Tries and install package, if possible.
    if(NOT package_FOUND AND NOT PIPINSTALL_NOINSTALL)
        _check_package_already_installed(pip pip_FOUND
            PYTHON_EXECUTABLE ${LOCALPYTHON}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
        )
        if(pip_FOUND)
            execute_process(
                COMMAND ${LOCALPYTHON} -m pip install ${ARGUMENTS} ${PACKAGE}
                WORKING_DIRECTORY ${WORKING_DIRECTORY}
                RESULT_VARIABLE PIPINSTALLATION_WORKED
                OUTPUT_VARIABLE PIP_OUTPUT
                ERROR_VARIABLE PIP_ERROR
            )

            if(NOT PIPINSTALLATION_WORKED EQUAL 0)
                message(STATUS "pip installation failed:")
                message(STATUS "${PIP_OUTPUT}")
                message(STATUS "${PIP_ERROR}")
            endif()
            _check_package_already_installed(${PACKAGE} package_FOUND
                PYTHON_EXECUTABLE ${LOCALPYTHON}
                WORKING_DIRECTORY ${WORKING_DIRECTORY}
            )
        endif()
    endif()
    # Figures out package location
    if(package_FOUND)
        execute_process(
            COMMAND ${LOCALPYTHON} -m pip show ${PACKAGE}
            WORKING_DIRECTORY ${WORKING_DIRECTORY}
            RESULT_VARIABLE RESULT
            ERROR_VARIABLE ERROR
            OUTPUT_VARIABLE OUTPUT
        )
        if(RESULT EQUAL 0)
            string(REGEX REPLACE
                ".*Location:[ ]*([^ \n]+).*\n" "\\1"
                ${PACKAGE}_LOCATION ${OUTPUT}
            )
            string(REGEX REPLACE
                ".*Version:.*([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1"
                ${PACKAGE}_VERSION ${OUTPUT}
            )
       endif()
    endif()

    set(${PACKAGE}_FIND_REQUIRED FALSE)
    if(PIPINSTALL_REQUIRED)
        set(${PACKAGE}_FIND_REQUIRED TRUE)
    endif()
    # Complete process of finding the package
    find_package_handle_standard_args(${PACKAGE}
        FOUND_VAR ${PACKAGE}_FOUND
        REQUIRED_VARS ${PACKAGE}_LOCATION ${PACKAGE}_PYTHON
        VERSION_VAR ${PACKAGE}_VERSION
    )
    if(${PACKAGE}_FOUND)
        foreach(value VERSION PYTHON LOCATION)
            set(${PACKAGE}_${value} ${${PACKAGE}_${value}} PARENT_SCOPE)
        endforeach()
    endif()
endfunction()
