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
if(PURIFY_MPI)
  add_example(global_epsilon_replicated_grids padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_global_epsilon_replicated_grids
    PRIVATE PURIFY_PADMM_ALGORITHM=2)
  add_example(local_epsilon_replicated_grids padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_replicated_grids
    PRIVATE PURIFY_PADMM_ALGORITHM=3)
endif()
add_example(sara_padmm_random_coverage LIBRARIES libpurify NOTEST)

add_example(clean_vla LIBRARIES libpurify NOTEST)

if(data AND TARGET casacore::ms)
  add_example(casa LIBRARIES libpurify)
endif()
