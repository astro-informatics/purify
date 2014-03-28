# Creates a python virtual environment in the build directory
# This python environment can be accessed via `LOCAL_PYTHON_EXECUTABLE`
# Furthermore, invoking pip_install(something LOCAL) will make sure to install in the local
# environment, leaving the system untouched.
# Finally, a special function `add_to_python_path` can be used to add paths to the local
# environment. In practice, it creates a file "pypaths" in the build directory, where each line is
# an additional directory.
# For debugging ease on unixes, a link to the virtual environment is created in the main build
# directory, called `localpython`.
find_package(PythonInterp)
include(utilities)
include(PythonPackage)

function(add_to_python_path PATH)
    if(NOT EXISTS ${PROJECT_BINARY_DIR}/pypaths)
        file(WRITE ${PROJECT_BINARY_DIR}/pypaths "${PATH}\n")
        return()
    endif()
    file(STRINGS ${PROJECT_BINARY_DIR}/pypaths ALLPATHS)
    list(FIND ALLPATHS "${PATH}" INDEX)
    if(INDEX EQUAL -1)
        file(APPEND ${PROJECT_BINARY_DIR}/pypaths "${PATH}\n")
    endif()
endfunction()

function(_create_virtualenv)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -m virtualenv
            --system-site-packages ${VIRTUALENV_DIRECTORY}
        RESULT_VARIABLE RESULT
        ERROR_VARIABLE ERROR
        OUTPUT_VARIABLE OUTPUT
    )
    if(NOT RESULT EQUAL 0)
        message(STATUS "${OUTPUT}")
        message(STATUS "${ERROR}")
        message(FATAL_ERROR "Could not create virtual environment")
    endif()
endfunction()

find_python_package(virtualenv REQUIRED)
set(VIRTUALENV_DIRECTORY ${CMAKE_BINARY_DIR}/external/virtualenv
    CACHE INTERNAL "Path to virtualenv" )

_create_virtualenv()

set(LOCAL_PYTHON_EXECUTABLE ${VIRTUALENV_DIRECTORY}/bin/python
    CACHE INTERNAL "Path to local python")
# Adds link to local python on unixes
symlink( "${VIRTUALENV_DIRECTORY}/bin/python"
    "${PROJECT_BINARY_DIR}/localpython")
# Adds a sitecustomize.py file that can easily add paths to the local python
string(REGEX REPLACE "([0-9]\\.[0-9]+)\\.[0-9]+" "\\1"
    version_string "${PYTHON_VERSION_STRING}")
set(CUSTOMIZE_FILE "${VIRTUALENV_DIRECTORY}/lib/python${version_string}")
if(NOT EXISTS "${CUSTOMIZE_FILE}/sitecustomize.py")
    file(WRITE "${CUSTOMIZE_FILE}/sitecustomize.py"
        "from sys import path\n"
        "from os.path import exists\n"
        "if exists('${PROJECT_BINARY_DIR}/pypaths'):\n"
        "    with open('${PROJECT_BINARY_DIR}/pypaths', 'r') as file:\n"
        "        newpaths = [u.rstrip().lstrip() for u in file.readlines()]\n"
        "    for dir in newpaths:\n"
        "        if dir not in path:\n"
        "            path.insert(0, dir)\n"
    )
endif()
