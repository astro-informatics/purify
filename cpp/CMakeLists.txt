# Setup logging
if(logging)
  set(PURIFY_DO_LOGGING 1)
  set(PURIFY_TEST_LOG_LEVEL off CACHE STRING "Level when logging tests")
  set_property(CACHE PURIFY_TEST_LOG_LEVEL PROPERTY STRINGS
    emerg off trace debug info notice warn err critical alert)
else()
  unset(PURIFY_DO_LOGGING)
  set(PURIFY_TEST_LOG_LEVEL off)
endif()

set(SOURCES MeasurementOperator.cc FFTOperator.cc clean.cc utilities.cc pfitsio.cc
  kernels.cc RMOperator.cc PSFOperator.cc wprojection.cc)
if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc)
endif()
add_library(cppurify ${SOURCES})

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

include_directories(SYSTEM ${FFTW3_INCLUDE_DIR} ${Sopt_INCLUDE_DIRS})

target_link_libraries(cppurify ${FFTW3_DOUBLE_LIBRARY} ${CCFits_LIBRARY} ${CFitsIO_LIBRARY})
if(TARGET casacore::ms)
  target_link_libraries(cppurify casacore::ms)
endif()

add_dependencies(cppurify lookup_dependencies)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/purify/")
configure_file(tests/directories.in.h "${PROJECT_BINARY_DIR}/include/purify/directories.h")
target_include_directories(cppurify PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/purify>
  $<INSTALL_INTERFACE:include/purify>)

add_subdirectory(example)
add_subdirectory(regressions)
if(tests)
  add_subdirectory(tests)
endif()
