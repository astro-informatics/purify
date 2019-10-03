include(AddExample)

include_directories(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES libpurify NOTEST)
add_example(rm_gridding_example LIBRARIES libpurify NOTEST)
add_example(image_wproj_chirp LIBRARIES libpurify NOTEST)
add_example(image_rm_window LIBRARIES libpurify NOTEST)
add_example(compare_wprojection LIBRARIES libpurify NOTEST)
add_example(generate_vis_data LIBRARIES libpurify NOTEST)

add_example(plot_wkernel LIBRARIES libpurify NOTEST)
add_example(plot_rmkernel LIBRARIES libpurify NOTEST)
add_example(wavelet_decomposition LIBRARIES libpurify NOTEST)
add_example(histogram_equalisation LIBRARIES libpurify NOTEST)

add_example(show_image LIBRARIES libpurify NOTEST)
add_example(sdmm_m31_simulation LIBRARIES libpurify NOTEST)

add_example(padmm_simulation LIBRARIES libpurify NOTEST)
add_example(padmm_reweighted_simulation LIBRARIES libpurify NOTEST)
add_example(sara_padmm_random_coverage LIBRARIES libpurify NOTEST)



add_example(padmm_real_data LIBRARIES libpurify NOTEST )

add_example(padmm_random_coverage LIBRARIES libpurify NOTEST )
if(PURIFY_ARRAYFIRE)
  add_example(padmm_random_coverage_gpu padmm_random_coverage.cc LIBRARIES libpurify NOTEST )
  target_compile_definitions(example_padmm_random_coverage_gpu PRIVATE PURIFY_GPU)
endif()
if(PURIFY_MPI)
  add_example(global_epsilon_replicated_grids_mpi padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_global_epsilon_replicated_grids_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=2)
  add_example(local_epsilon_replicated_grids_mpi padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_replicated_grids_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=3)
  add_example(local_epsilon_distributed_grids_mpi padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_distributed_grids_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=1)
  add_example(global_epsilon_replicated_grids_real_data_mpi padmm_mpi_real_data.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_global_epsilon_replicated_grids_real_data_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=2)
  add_example(local_epsilon_replicated_grids_real_data_mpi padmm_mpi_real_data.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_replicated_grids_real_data_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=3)
  add_example(local_epsilon_distributed_grids_real_data_mpi padmm_mpi_real_data.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_distributed_grids_real_data_mpi
    PRIVATE PURIFY_PADMM_ALGORITHM=1)
  add_example(time_w_algos LIBRARIES libpurify NOTEST)
endif()
if(PURIFY_MPI AND PURIFY_ARRAYFIRE)
  add_example(global_epsilon_replicated_grids_gpu padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_global_epsilon_replicated_grids_gpu
    PRIVATE PURIFY_PADMM_ALGORITHM=2 PURIFY_GPU)
  add_example(local_epsilon_replicated_grids_gpu padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_replicated_grids_gpu
    PRIVATE PURIFY_PADMM_ALGORITHM=3 PURIFY_GPU)
  add_example(local_epsilon_distributed_grids_gpu padmm_mpi_random_coverage.cc NOTEST
    LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
  target_compile_definitions(example_local_epsilon_distributed_grids_gpu
    PRIVATE PURIFY_PADMM_ALGORITHM=1 PURIFY_GPU)
  if(TARGET casacore::ms)
    add_example(global_epsilon_replicated_grids_real_data_gpu padmm_mpi_real_data.cc NOTEST
      LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    target_compile_definitions(example_global_epsilon_replicated_grids_real_data_gpu
      PRIVATE PURIFY_PADMM_ALGORITHM=2 PURIFY_GPU)
    add_example(local_epsilon_replicated_grids_real_data_gpu padmm_mpi_real_data.cc NOTEST
      LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    target_compile_definitions(example_local_epsilon_replicated_grids_real_data_gpu
      PRIVATE PURIFY_PADMM_ALGORITHM=3 PURIFY_GPU)
    add_example(local_epsilon_distributed_grids_real_data_gpu padmm_mpi_real_data.cc NOTEST
      LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    target_compile_definitions(example_local_epsilon_distributed_grids_real_data_gpu
      PRIVATE PURIFY_PADMM_ALGORITHM=1 PURIFY_GPU)
  endif()
endif()


if(data AND TARGET casacore::ms)
  add_example(casa LIBRARIES libpurify)
endif()
