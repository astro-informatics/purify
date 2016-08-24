include(AddExample)

include_directorieS(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES cppurify)
add_example(rm_gridding_example LIBRARIES cppurify NOTEST)
add_example(generate_vis_data LIBRARIES cppurify NOTEST)

add_example(time_gridding_degridding LIBRARIES cppurify NOTEST)

add_example(sdmm_random_coverage LIBRARIES cppurify NOTEST)
add_example(sdmm_vla LIBRARIES cppurify NOTEST)

add_example(resample LIBRARIES cppurify)
add_example(sparsify LIBRARIES cppurify)
add_example(generate_chirp LIBRARIES cppurify)
add_example(w_projection_example LIBRARIES cppurify NOTEST)

add_example(sdmm_m31_simulation LIBRARIES cppurify NOTEST)

add_example(padmm_m31_simulation LIBRARIES cppurify NOTEST)
add_example(padmm_clean_m31_simulation LIBRARIES cppurify NOTEST)
add_example(padmm_ms_clean_m31_simulation LIBRARIES cppurify NOTEST)

add_example(padmm_random_coverage LIBRARIES cppurify NOTEST)

add_example(sara_padmm_random_coverage LIBRARIES cppurify NOTEST)

add_example(clean_vla LIBRARIES cppurify NOTEST)

if(data AND TARGET casacore::ms)
  add_example(casa LIBRARIES cppurify)
endif()
