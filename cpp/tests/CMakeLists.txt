include_directories("${PROJECT_SOURCE_DIR}/cpp")
add_catch_test(measurement_operator LIBRARIES cppurify)
add_catch_test(FFT_operator LIBRARIES cppurify)
add_catch_test(purify_fitsio LIBRARIES cppurify)