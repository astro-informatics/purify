cmake_minimum_required(VERSION 2.8)
project(Purify C CXX)

# Location of extra cmake includes for the project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_files)

# Downloads and installs GreatCMakeCookOff
# It contains a number of cmake recipes
include(LookUp-GreatCMakeCookOff)

option(tests          "Enable testing"                                  on)
option(examples       "Compile examples"                                on)
option(data           "Download measurement set for testing"            on)
option(openmp         "Enable OpenMP"                                   on)
option(logging        "Enable logging"                                  on)

# Set version and build id of this package
include(version)

# we are doing c++11
include(AddCPP11Flags)

# sets up rpath so libraries can be found
include(rpath)

# adds logging variables
include(logging)

# include exernal dependencies
include(dependencies)
if(EXISTS "${EXTERNAL_ROOT}/lib/")
    install(DIRECTORY "${EXTERNAL_ROOT}/lib/"
        DESTINATION "${LIBRARY_INSTALL_PATH}"
        PATTERN "pkgconfig" EXCLUDE
        PATTERN "*.a" EXCLUDE
    )
endif()

if(tests AND data)
    include(data_dependencies)
endif()

if(tests)  # Adds ctest
    enable_testing()
    include(AddCatchTest)
endif()

add_subdirectory(cpp)

# Exports Purify so other packages can access it
include(export_purify)
