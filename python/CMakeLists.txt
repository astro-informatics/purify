if(python)
  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS})
  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")
  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  function(add_python_module NAME) 
    add_library (py${NAME} SHARED ${ARGN})
    set_target_properties(py${NAME} PROPERTIES OUTPUT_NAME ${NAME})
    set_target_properties(py${NAME} PROPERTIES PREFIX "")
    set_target_properties(py${NAME} PROPERTIES SUFFIX ".so")
    target_link_libraries(py${NAME} ${PYTHON_LIBRARIES})
    # Change path to something such that we can import it in tests.
    set_target_properties(py${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYPURIFY_DIRECTORY})
  endfunction()

  # Copy python files to output package
  file(GLOB input_sources *.py)
  foreach(input ${input_sources})
    add_custom_command(
      TARGET pypurify
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${input} ${PYPURIFY_DIRECTORY}
    )
  endforeach()

  # Installation
  install(TARGETS pypurify DESTINATION ${PYTHON_PKG_DIR}/purify)
  install(FILES ${input_sources} DESTINATION ${PYTHON_PKG_DIR}/purify)
endif()

add_subdirectory(tests)
