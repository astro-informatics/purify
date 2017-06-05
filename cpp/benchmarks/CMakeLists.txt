include_directories("${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")

add_benchmark(measurement_operator LIBRARIES libpurify)
