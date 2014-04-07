# Look for external software
find_package(FFTW3 REQUIRED DOUBLE)
find_package(TIFF REQUIRED)
find_package(BLAS REQUIRED)
# find_package blas does not look for cblas.h
if(NOT BLAS_INCLUDE_DIR)
  find_path(BLAS_INCLUDE_DIR cblas.h)
endif()

# Look up packages: if not found, installs them
include(PackageLookup)
lookup_package(Sopt REQUIRED ARGUMENTS GIT_TAG features/cmake)
lookup_package(CFitsIO REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")

if(openmp)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  else()
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()

if(python)
    function(find_or_fail package what)
        find_python_package(${package})
        if(NOT ${package}_FOUND)
            message("*********")
            message("${package} is required to ${what}")
            message("It can likely be installed with pip")
            message("*********")
            message(FATAL_ERROR "Aborting")
        endif()
    endfunction()
    # Python interpreter + libraries
    find_package(CoherentPython)
    # Function to install python files in python path ${PYTHON_PKG_DIR}
    include(PythonInstall)
    # Ability to find installed python packages
    include(PythonPackage)
    # Look for/install cython and nose
    # Only required for building
    find_or_fail(cython "build Purify's python bindings")
    # Also required for production
    find_or_fail(numpy "by Purify's python bindings")
    find_or_fail(scipy "by Purify's python bindings")
    find_or_fail(pandas "by Purify's python bindings")

    if(tests)
        # unit-test package
        find_or_fail(nose "to run the unit-tests for the python bindings")
        # python environment within which ctest will run the test.
        # ensures ctest finds the packages in the build tree first,
        # as opposed to install packages.
        find_or_fail(virtualenv "to run the unit-tests for the python bindings")
        include(PythonVirtualEnv)
    endif()

    # Finds additional info, like libraries, include dirs...
    find_package(Numpy REQUIRED)
endif()
