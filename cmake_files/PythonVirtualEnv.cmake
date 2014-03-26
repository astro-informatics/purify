# Creates a python virtual environment in the build directory
# This python environment can be accessed via `LOCAL_PYTHON_EXECUTABLE`
# Furthermore, invoking pip_install(something LOCAL) will make sure to install in the local
# environment, leaving the system untouched.
# Finally, a special function `add_to_python_path` can be used to add paths to the local
# environment. In practice, it creates a file "pypaths" in the build directory, where each line is
# an additional directory.
# For debugging ease on unixes, a link to the virtual environment is created in the main build
# directory, called `localpython`.
include(PythonPackage)

pip_install(virtualenv PREFIX
    ${PROJECT_BINARY_DIR}/external/python
    REQUIRED
)

set(VIRTUALENV_DIRECTORY ${CMAKE_BINARY_DIR}/external/virtualenv
    CACHE INTERNAL "Path to virtualenv" )
if(NOT EXISTS ${VIRTUALENV_DIRECTORY})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${VIRTUALENV_DIRECTORY}
        WORKING_DIRECTORY ${PYTHON_EXTERNAL_PATH}/pylibs
        OUTPUT_QUIET
    )
endif()

if(NOT EXISTS ${VIRTUALENV_DIRECTORY}/bin/python)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE}
                    -m virtualenv
                    --system-site-packages ${VIRTUALENV_DIRECTORY}
        WORKING_DIRECTORY ${virtualenv_LOCATION}
        RESULT_VARIABLE RESULT
        OUTPUT_QUIET
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Could not create virtual environment")
    endif()
endif()
set(LOCAL_PYTHON_EXECUTABLE ${VIRTUALENV_DIRECTORY}/bin/python
    CACHE INTERNAL "Path to local python")

# Adds a link to the local python environment
if(NOT EXISTS ${PROJECT_BINARY_DIR}/localpython)
    if(NOT WIN32)
        execute_process(
            COMMAND ln -s ${LOCAL_PYTHON_EXECUTABLE} localpython
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            OUTPUT_QUIET
        )
    endif()
endif()

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
        "    path.extend([u for u in newpaths if len(u) and exists(u)])\n"
    )
endif()

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
