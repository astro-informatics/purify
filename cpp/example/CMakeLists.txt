 if(EIGEN3_INCLUDE_DIR)
   include_directories(${EIGEN3_INCLUDE_DIR})
 endif()
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
 
 add_executable(gridding gridding.cc)
 target_link_libraries(gridding cppurify)
