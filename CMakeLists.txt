cmake_minimum_required(VERSION 2.8)
project(Purify C)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Set version and build id of this package
include(version_and_build_id)
version_and_build_id(0.1)

option(tests          "Enable testing."                         on)


# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes 
include(LookUp-GreatCMakeCookOff)

# include exernal dependencies
include(dependencies)
if(tests)
  enable_testing()
endif()

add_subdirectory(src)
add_subdirectory(include)

