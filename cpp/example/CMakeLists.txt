include(AddExample)

include_directorieS(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES libpurify)
add_example(rm_gridding_example LIBRARIES libpurify NOTEST)
add_example(generate_vis_data LIBRARIES libpurify NOTEST)

add_example(time_gridding_degridding LIBRARIES libpurify NOTEST)

add_example(sdmm_random_coverage LIBRARIES libpurify NOTEST)
add_example(sdmm_vla LIBRARIES libpurify NOTEST)

add_example(resample LIBRARIES libpurify)

add_example(sdmm_m31_simulation LIBRARIES libpurify NOTEST)

add_example(padmm_simulation LIBRARIES libpurify NOTEST)
add_example(padmm_reweighted_simulation LIBRARIES libpurify NOTEST)

add_example(padmm_random_coverage LIBRARIES libpurify NOTEST)

add_example(sara_padmm_random_coverage LIBRARIES libpurify NOTEST)

add_example(clean_vla LIBRARIES libpurify NOTEST)

if(data AND TARGET casacore::ms)
  add_example(casa LIBRARIES libpurify)
endif()
