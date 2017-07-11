include_directories("${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")

add_benchmark(measurement_operator utilities.cc LIBRARIES libpurify)

if(dompi)
  add_executable(mpi_benchmarks main.cc utilities.cc measurement_operator_mpi.cc)
  target_link_libraries(mpi_benchmarks ${MPI_LIBRARIES} gbenchmark libpurify)
  target_include_directories(mpi_benchmarks PUBLIC "${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
endif()
