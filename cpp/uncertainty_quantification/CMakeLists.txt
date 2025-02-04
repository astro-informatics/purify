add_executable(purify_UQ uq_main.cc)
target_link_libraries(purify_UQ libpurify)
set_target_properties(purify_UQ PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

install(TARGETS purify_UQ
  EXPORT PurifyTargets
  DESTINATION share/cmake/Purify
  RUNTIME DESTINATION bin
  )