# Scripts to run purify from build directory. Good for testing/debuggin.
include(EnvironmentScript)
# Look up packages: if not found, installs them
include(PackageLookup)

# Look for external software
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads)
  if(THREADS_FOUND)
    add_compile_options(-pthread)
  endif(THREADS_FOUND)
endif()

# Always find open-mp, since it may be used by sopt
find_package(OpenMP)
if(OPENMP_FOUND)
  add_library(openmp::openmp INTERFACE IMPORTED GLOBAL)
  set_target_properties(openmp::openmp PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
    INTERFACE_LINK_LIBRARIES  "${OpenMP_CXX_FLAGS}")
endif()
if(openmp AND NOT OPENMP_FOUND)
    message(STATUS "Could not find OpenMP. Compiling without.")
endif()
set(PURIFY_OPENMP_FFTW FALSE)
if(openmp AND OPENMP_FOUND)
  set(PURIFY_OPENMP TRUE)
  find_package(FFTW3 REQUIRED DOUBLE SERIAL COMPONENTS OPENMP)
  set(FFTW3_DOUBLE_LIBRARY fftw3::double::serial)
  if(TARGET fftw3::double::openmp)
    list(APPEND FFTW3_DOUBLE_LIBRARY fftw3::double::openmp)
    set(PURIFY_OPENMP_FFTW TRUE)
  endif()
else()
  set(PURIFY_OPENMP FALSE)
  find_package(FFTW3 REQUIRED DOUBLE)
  set(FFTW3_DOUBLE_LIBRARY fftw3::double::serial)
endif()

find_package(TIFF REQUIRED)


if(data AND tests)
  lookup_package(Boost REQUIRED COMPONENTS filesystem)
else()
  lookup_package(Boost REQUIRED)
endif()

lookup_package(Eigen3 REQUIRED DOWNLOAD_BY_DEFAULT ARGUMENTS URL "https://bitbucket.org/LukePratley/eigen/get/3.2.tar.gz" MD5 "f38e33505afbf4659cda191bbc992ca9")

if(logging)
  lookup_package(spdlog REQUIRED)
endif()

# Look up packages: if not found, installs them
# Unless otherwise specified, if purify is not on master, then sopt will be
# downloaded from development branch.
if(NOT Sopt_GIT_TAG)
  set(Sopt_GIT_TAG master CACHE STRING "Branch/tag when downloading sopt")
endif()
if(NOT Sopt_GIT_REPOSITORY)
  set(Sopt_GIT_REPOSITORY https://www.github.com/basp-group/sopt.git
    CACHE STRING "Location when downloading sopt")
endif()
  
lookup_package(
    Sopt REQUIRED ARGUMENTS
    GIT_REPOSITORY ${Sopt_GIT_REPOSITORY}
    GIT_TAG ${Sopt_GIT_TAG})

lookup_package(CFitsIO REQUIRED ARGUMENTS CHECKCASA)
lookup_package(CCFits REQUIRED)

find_package(CasaCore OPTIONAL_COMPONENTS ms)

# Add script to execute to make sure libraries in the build tree can be found
add_to_ld_path("${EXTERNAL_ROOT}/lib")
