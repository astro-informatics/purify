include(AddExample)

include_directorieS(SYSTEM ${Sopt_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/cpp")

add_example(gridding LIBRARIES cppurify)
add_example(gridding_mem LIBRARIES cppurify)
add_example(sdmm LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(sdmm_vla LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
add_example(admm LIBRARIES cppurify libpurify ${Sopt_CPP_LIBRARY})
