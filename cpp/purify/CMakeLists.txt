configure_file(config.in.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(SOURCES MeasurementOperator.cc FFTOperator.cc clean.cc utilities.cc pfitsio.cc
  kernels.cc RMOperator.cc PSFOperator.cc wprojection.cc)
if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc)
endif()
add_library(libpurify SHARED ${SOURCES})
set(version "${Purify_VERSION_MAJOR}.${Purify_VERSION_MINOR}.${Purify_VERSION_PATCH}")
set_target_properties(libpurify PROPERTIES VERSION ${version} SOVERSION ${version})

target_include_directories(libpurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/cpp>
  $<INSTALL_INTERFACE:include/>)


if(EIGEN3_INCLUDE_DIR)
  target_include_directories(libpurify SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR})
endif()
if(Boost_INCLUDE_DIR)
  target_include_directories(libpurify SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
endif()
if(CFitsIO_INCLUDE_DIR)
  target_include_directories(libpurify SYSTEM PUBLIC ${CFitsIO_INCLUDE_DIR})
endif()
if(CCFits_INCLUDE_DIR)
  target_include_directories(libpurify SYSTEM PUBLIC ${CCFits_INCLUDE_DIR}/.. ${CCFits_INCLUDE_DIR})
endif()
target_link_libraries(libpurify ${FFTW3_DOUBLE_LIBRARY} ${CCFits_LIBRARY} ${CFitsIO_LIBRARY} ${Sopt_CPP_LIBRARY})
if(TARGET casacore::ms)
  target_link_libraries(libpurify casacore::ms)
endif()
if(TARGET openmp::openmp)
  target_link_libraries(libpurify openmp::openmp)
endif()

add_dependencies(libpurify lookup_dependencies)
