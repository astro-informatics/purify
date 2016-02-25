# Scripts to run purify from build directory. Good for testing/debuggin.
include(EnvironmentScript)
# Look up packages: if not found, installs them
include(PackageLookup)

# Look for external software
find_package(FFTW3 REQUIRED DOUBLE)
find_package(TIFF REQUIRED)
find_package(CBLAS REQUIRED)
set(PURIFY_BLAS_H "${BLAS_INCLUDE_FILENAME}")

lookup_package(Eigen3 REQUIRED)
lookup_package(Boost REQUIRED)

# Look up packages: if not found, installs them
# Unless otherwise specified, if purify is not on master, then sopt will be
# downloaded from development branch.
if(NOT sopt_tag)
    execute_process(COMMAND git branch
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        RESULT_VARIABLE got_branch
        OUTPUT_VARIABLE current_branch)
    if(NOT got_branch)
        set(sopt_tag master)
    endif()
    if(NOT current_branch STREQUAL master)
        set(sopt_tag development-c-and-cpp)
    endif()
    set(sopt_tag ${sopt_tag} CACHE STRING "Branch/tag when downloading sopt")
endif()
if(NOT Sopt_FOUND)
  message(STATUS "If downloading Sopt locally, then it will be branch ${sopt_tag}")
endif()
lookup_package(
    Sopt REQUIRED ARGUMENTS
    GIT_REPOSITORY git@github.com:astro-informatics/sopt.git
    GIT_TAG ${sopt_tag})

lookup_package(CFitsIO REQUIRED ARGUMENTS CHECKCASA)
lookup_package(CCFits REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")

if(openmp)
  find_package(OpenMP)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  else()
    message(STATUS "Could not find OpenMP. Compiling without.")
  endif()
endif()

find_package(Doxygen)

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
