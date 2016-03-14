include(AddExample)

include_directorieS(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES cppurify)
add_example(gridding_mem LIBRARIES cppurify)
add_example(sdmm_ami_coverage LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(sdmm_random_coverage LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(sdmm_vla LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(admm LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})

add_example(resample LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(sparsify LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(generate_chirp LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(w_projection_example LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
