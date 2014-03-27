if(CFitsIO_ARGUMENTS)
    cmake_parse_arguments(CFitsIO "" "URL" ""
        ${CFitsIO_ARGUMENTS})
endif()
if(NOT CFitsIO_URL)
    set(CFitsIO_URL ftp://heasarc.gsfc.nasa.gov/software/fitsio/c/cfitsio_latest.tar.gz)
endif()
ExternalProject_Add(
    CFitsIO
    PREFIX ${EXTERNAL_ROOT}
    URL ${CFitsIO_URL}
    BUILD_IN_SOURCE  1
    CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_ROOT}
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
add_recursive_cmake_step(CFitsIO CFitsIO_FOUND DEPENDEES install)
