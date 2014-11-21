# Looks up [CFitsIO](http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html)
#
# - URL: Defaults to ftp://heasarc.gsfc.nasa.gov/software/fitsio/c/cfitsio_latest.tar.gz
#
if(CFitsIO_ARGUMENTS)
    cmake_parse_arguments(CFitsIO "CHECKCASA" "URL" ""
        ${CFitsIO_ARGUMENTS})
endif()
# Figures out this is a casa install from existence of CASAPATH environment
# variables
if(CFitsIO_CHECKCASA AND NOT "$ENV{CASAPATH}" STREQUAL "" AND NOT APPLE)
    set(using_casa TRUE)
else()
    set(using_casa FALSE)
endif()

if(NOT CFitsIO_URL AND NOT using_casa)
    set(CFitsIO_URL
        ftp://heasarc.gsfc.nasa.gov/software/fitsio/c/cfitsio_latest.tar.gz)
elseif(NOT CFistIO_URL)
    set(CFitsIO_URL
        https://svn.cv.nrao.edu/casa/devel/cfitsio+patch.tar.gz)
endif()

if(using_casa)
    # Library already exists, but is missing headers
    # Only copies headers to appropriate location
    # NOTE: it might be that the Mac version packages a different cfitsio. In
    # anycase, this doesn't work on mac
    set(script "${EXTERNAL_ROOT}/src/cfitsio.cmake")
    string(FIND "$ENV{CASAPATH}" " " endpath)
    string(SUBSTRING "$ENV{CASAPATH}" 0 ${endpath} casapath)
    if(APPLE)
       set(destination "${casapath}/Frameworks/Headers/cfitsio")
    else()
       set(destination "${casapath}/include/cfitsio")
    endif()
    file(WRITE "${script}"
        "file(GLOB headers cfitsio/*.h)\n"
        "foreach(header \${headers})\n"
        "    get_filename_component(filename \"\${header}\" NAME)\n"
        "    configure_file(\n"
        "        \${header} \"${destination}/\${filename}\" COPY_ONLY\n"
        "    )\n"
        "endforeach()"
    )
    find_program(TAR_EXECUTABLE tar)
    if(NOT TAR_EXECUTABLE)
        message(FATAL_ERROR "Cannot find tar executable in path")
    endif()
    ExternalProject_Add(
        CFitsIO
        PREFIX ${EXTERNAL_ROOT}
        URL ${CFitsIO_URL}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${TAR_EXECUTABLE} -xf cfitsio3030.tar.gz
        INSTALL_COMMAND ${CMAKE_COMMAND} -P ${script}
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
    )
else()
    ExternalProject_Add(
        CFitsIO
        PREFIX ${EXTERNAL_ROOT}
        URL ${CFitsIO_URL}
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./configure --prefix=${EXTERNAL_ROOT}
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
    )
endif()
add_recursive_cmake_step(CFitsIO DEPENDEES install)
set(CFitsIO_INCLUDE_DIR "")
set(CFitsIO_LIBRARY "")
