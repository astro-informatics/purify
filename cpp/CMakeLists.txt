if(EIGEN3_INCLUDE_DIR)
  include_directories(${EIGEN3_INCLUDE_DIR})
endif()
add_library(cppurify MeasurementOperator.cc FFTOperator.cc utilities.cc)
add_dependencies(cppurify lookup_dependencies)

add_subdirectory(example)

if(tests)
  add_subdirectory(tests)
endif()
