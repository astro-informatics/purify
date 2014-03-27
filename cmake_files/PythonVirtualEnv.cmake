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

function(_have_virtualenv_version VERSION OUTVAR)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c
            "import virtualenv; print(virtualenv.__version__)"
        RESULT_VARIABLE RESULT OUTPUT_VARIABLE OUTPUT
        ERROR_VARIABLE ERROR
    )
    if(NOT RESULT EQUAL 0)
        set(${OUTVAR} FALSE PARENT_SCOPE)
    elseif(${OUTPUT} VERSION_EQUAL ${VERSION})
        set(${OUTVAR} TRUE PARENT_SCOPE)
    elseif(${OUTPUT} VERSION_GREATER ${VERSION})
        set(${OUTVAR} TRUE PARENT_SCOPE)
    endif()
endfunction()

function(_download_virtualenv VERSION)
    set(source_dir ${PROJECT_BINARY_DIR}/external/src/virtualenv)
    mkdir("${source_dir}/build")
    if(NOT EXISTS ${source_dir}/CMakeLists.txt)
        file(WRITE ${source_dir}/CMakeLists.txt
            "cmake_minimum_required(VERSION 2.8)\n"
            "project(virtualenv C)\n"
            "include(ExternalProject)\n"
            "ExternalProject_Add(virtualenv\n"
            "    GIT_REPOSITORY https://github.com/pypa/virtualenv/\n"
            "    GIT_TAG 1.11.4\n"
            "    BUILD_IN_SOURCE 1\n"
            "    CONFIGURE_COMMAND \"\"\n"
            "    BUILD_COMMAND ${PYTHON_EXECUTABLE} virtualenv.py "
                    "--system-site-packages "
                    "${VIRTUALENV_DIRECTORY}\n"
            "    INSTALL_COMMAND \"\"\n"
            ")\n"
        )
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} ${source_dir}
        WORKING_DIRECTORY ${source_dir}/build
        RESULT_VARIABLE RESULT
        ERROR_VARIABLE ERROR
        OUTPUT_VARIABLE OUTPUT
    )
    if(NOT RESULT EQUAL 0)
        message(STATUS "${OUTPUT}")
        message(STATUS "${ERROR}")
        message(FATAL_ERROR "Could not install virtual environment")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${source_dir}/build
        RESULT_VARIABLE RESULT
        ERROR_VARIABLE ERROR
        OUTPUT_VARIABLE OUTPUT
    )
    if(NOT RESULT EQUAL 0)
        message(STATUS "${OUTPUT}")
        message(STATUS "${ERROR}")
        message(FATAL_ERROR "Could not install virtual environment")
    endif()
endfunction()

function(_use_current_virtualenv)
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

function(add_virtualenv)
    _have_virtualenv_version(1.11 have_virtualenv)
    set(VIRTUALENV_DIRECTORY ${CMAKE_BINARY_DIR}/external/virtualenv
        CACHE INTERNAL "Path to virtualenv" )

    if(have_virtualenv)
        _use_current_virtualenv()
    else()
        _download_virtualenv(1.11.4)
    endif()
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
            "    path.extend([u for u in newpaths if len(u) and exists(u)])\n"
        )
    endif()
endfunction()

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
