include_directories("${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include" "${benchmark_DIR}/include")

add_executable(measurement_operator utilities.cc measurement_operator.cc)
target_link_libraries(measurement_operator benchmark::benchmark libpurify)

add_executable(algorithms utilities.cc algorithms.cc)
target_link_libraries(algorithms benchmark::benchmark libpurify)

add_executable(wavelet_operator utilities.cc wavelet_operator.cc)
target_link_libraries(wavelet_operator benchmark::benchmark libpurify)

add_executable(fft utilities.cc fft.cc)
target_link_libraries(fft benchmark::benchmark libpurify)

add_executable(degridding utilities.cc degridding.cc)
target_link_libraries(degridding benchmark::benchmark libpurify)

# Skip ArrayFire benchmarks for now, add back later if needed
# if(doaf)
#   add_benchmark(measurement_operator_af utilities.cc LIBRARIES libpurify)
#   add_benchmark(measurement_operator_cpu measurement_operator_af.cc utilities.cc LIBRARIES libpurify)
#   target_compile_definitions(benchmark_measurement_operator_cpu PRIVATE PURIFY_CPU)
# endif()

if(dompi)
  add_executable(mpi_benchmark_MO main.cc utilities.cc measurement_operator_mpi.cc)
  target_link_libraries(mpi_benchmark_MO ${MPI_LIBRARIES} benchmark::benchmark libpurify)
  #target_include_directories(mpi_benchmark_MO PUBLIC "${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
  add_executable(mpi_benchmark_MO_wproj main.cc utilities.cc measurement_operator_wproj.cc)
  target_link_libraries(mpi_benchmark_MO_wproj ${MPI_LIBRARIES} benchmark::benchmark libpurify)
  #target_include_directories(mpi_benchmark_MO_wproj PUBLIC "${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
  add_executable(mpi_benchmark_algorithms main.cc utilities.cc algorithms_mpi.cc)
  target_link_libraries(mpi_benchmark_algorithms ${MPI_LIBRARIES} benchmark::benchmark libpurify)
  #target_include_directories(mpi_benchmark_PADMM PUBLIC "${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
  add_executable(mpi_benchmark_WLO main.cc utilities.cc wavelet_operator_mpi.cc)
  target_link_libraries(mpi_benchmark_WLO ${MPI_LIBRARIES} benchmark::benchmark libpurify)
  #target_include_directories(mpi_benchmark_WLO PUBLIC "${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
endif()
