 if(EIGEN3_INCLUDE_DIR)
   include_directories(${EIGEN3_INCLUDE_DIR})
 endif()
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
 
 add_executable(gridding gridding.cc)
  add_executable(gridding_mem gridding_mem.cc)
 target_link_libraries(gridding cppurify)
 target_link_libraries(gridding_mem cppurify)
