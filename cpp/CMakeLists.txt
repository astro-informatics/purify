# Setup logging
set(PURIFY_LOGGER_NAME "purify" CACHE STRING "NAME of the logger")
set(PURIFY_COLOR_LOGGING true CACHE BOOL "Whether to add color to the log")
if(logging)
  set(PURIFY_DO_LOGGING 1)
  set(PURIFY_TEST_LOG_LEVEL critical CACHE STRING "Level when logging tests")
  set_property(CACHE PURIFY_TEST_LOG_LEVEL PROPERTY STRINGS
    off critical error warn info debug trace)
else()
  unset(PURIFY_DO_LOGGING)
  set(PURIFY_TEST_LOG_LEVEL off)
  set_property(CACHE SOPT_TEST_LOG_LEVEL PROPERTY STRINGS off)
endif()

set(version ${Purify_VERSION})
string(REGEX REPLACE "\\." ";" version "${Purify_VERSION}")
list(GET version 0 Purify_VERSION_MAJOR)
list(GET version 1 Purify_VERSION_MINOR)
list(GET version 2 Purify_VERSION_PATCH)

configure_file(config.in.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(SOURCES MeasurementOperator.cc FFTOperator.cc clean.cc utilities.cc pfitsio.cc
  kernels.cc RMOperator.cc PSFOperator.cc wprojection.cc AlgorithmUpdate.cc cmdl.cc)
if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc)
endif()
add_library(cppurify ${SOURCES})

target_include_directories(cppurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
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
if(Sopt_INCLUDE_DIRS)
  target_include_directories(cppurify SYSTEM PUBLIC ${Sopt_INCLUDE_DIRS})
endif()
target_link_libraries(cppurify ${FFTW3_DOUBLE_LIBRARY} ${CCFits_LIBRARY} ${CFitsIO_LIBRARY} ${Sopt_CPP_LIBRARY})
if(TARGET casacore::ms)
  target_link_libraries(cppurify casacore::ms)
endif()
if(TARGET openmp::openmp)
  target_link_libraries(cppurify openmp::openmp)
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
