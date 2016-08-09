include(AddExample)

include_directorieS(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES cppurify)
add_example(gridding_mem LIBRARIES cppurify NOTEST)
add_example(rm_gridding_example LIBRARIES cppurify NOTEST)
add_example(generate_vis_data LIBRARIES cppurify NOTEST)

add_example(time_gridding_degridding LIBRARIES cppurify libpurify NOTEST)

add_example(sdmm_rm_simulation LIBRARIES cppurify libpurify)
add_example(sdmm_ami_coverage LIBRARIES cppurify libpurify NOTEST)
add_example(sdmm_random_coverage LIBRARIES cppurify libpurify NOTEST)
add_example(sdmm_vla LIBRARIES cppurify libpurify NOTEST)

add_example(admm LIBRARIES cppurify libpurify NOTEST)

add_example(resample LIBRARIES cppurify libpurify)
add_example(sparsify LIBRARIES cppurify libpurify)
add_example(generate_chirp LIBRARIES cppurify libpurify)
add_example(w_projection_example LIBRARIES cppurify libpurify NOTEST)

add_example(sdmm_m31_simulation LIBRARIES cppurify libpurify NOTEST)
add_example(sdmm_clean_m31_simulation LIBRARIES cppurify libpurify NOTEST)
add_example(sdmm_ms_clean_m31_simulation LIBRARIES cppurify libpurify NOTEST)

add_example(padmm_m31_simulation LIBRARIES cppurify libpurify NOTEST)
add_example(padmm_clean_m31_simulation LIBRARIES cppurify libpurify NOTEST)
add_example(padmm_ms_clean_m31_simulation LIBRARIES cppurify libpurify NOTEST)

add_example(padmm_ami_coverage LIBRARIES cppurify libpurify NOTEST)
add_example(padmm_random_coverage LIBRARIES cppurify libpurify NOTEST)
add_example(padmm_vla LIBRARIES cppurify libpurify NOTEST)


add_example(padmm_atca LIBRARIES cppurify libpurify NOTEST)
add_example(padmm_cygA LIBRARIES cppurify libpurify NOTEST)
add_example(sara_padmm_random_coverage LIBRARIES cppurify libpurify NOTEST)

add_example(clean_vla LIBRARIES cppurify libpurify NOTEST)

if(TARGET casacore::ms)
  add_example(purify_main LIBRARIES cppurify libpurify NOTEST)
endif()
if(data AND TARGET casacore::ms)
  add_example(casa LIBRARIES cppurify)
endif()
