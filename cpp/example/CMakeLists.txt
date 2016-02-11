include_directories("${PROJECT_SOURCE_DIR}/cpp")

 add_executable(gridding gridding.cc)
 target_link_libraries(gridding cppurify)
