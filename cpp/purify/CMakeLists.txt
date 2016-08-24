configure_file(config.in.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(SOURCES MeasurementOperator.cc FFTOperator.cc clean.cc utilities.cc pfitsio.cc
  kernels.cc RMOperator.cc PSFOperator.cc wprojection.cc)
if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc AlgorithmUpdate.cc cmdl.cc)
endif()
add_library(cppurify SHARED ${SOURCES})

target_include_directories(cppurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/cpp>
  $<INSTALL_INTERFACE:include/>)


if(EIGEN3_INCLUDE_DIR)
  target_include_directories(cppurify SYSTEM PUBLIC ${EIGEN3_INCLUDE_DIR})
endif()
if(Boost_INCLUDE_DIR)
  target_include_directories(cppurify SYSTEM PUBLIC ${Boost_INCLUDE_DIR})
endif()
if(CFitsIO_INCLUDE_DIR)
  target_include_directories(cppurify SYSTEM PUBLIC ${CFitsIO_INCLUDE_DIR})
endif()
if(CCFits_INCLUDE_DIR)
  target_include_directories(cppurify SYSTEM PUBLIC ${CCFits_INCLUDE_DIR}/.. ${CCFits_INCLUDE_DIR})
endif()
target_link_libraries(cppurify ${FFTW3_DOUBLE_LIBRARY} ${CCFits_LIBRARY} ${CFitsIO_LIBRARY} ${Sopt_CPP_LIBRARY})
if(TARGET casacore::ms)
  target_link_libraries(cppurify casacore::ms)
endif()
if(TARGET openmp::openmp)
  target_link_libraries(cppurify openmp::openmp)
endif()

add_dependencies(cppurify lookup_dependencies)
