if(tests AND python)
  # Creates the "tester", a python executable that knows where to find the purify package in the
  # build directory. This makes it easier to run nose or what not without having to modify the
  # pythonpath. Considering how brittle messing with environment variables can be, this is the best
  # option.
  get_filename_component(PATH_TO_PURIFY ${PYPURIFY_DIRECTORY} PATH)
  add_executable(tester tester.c)
  set_target_properties(tester PROPERTIES COMPILE_DEFINITIONS "PATH=\"${PATH_TO_PURIFY}\"")
  target_link_libraries(tester ${PYTHON_LIBRARIES})
  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS})
  get_target_property(TESTER_COMMAND tester LOCATION)

  # A function to add a nose test, using the tester python executable.
  function(add_nose_test SOURCE)
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)

    add_test(
      NAME ${SOURCE_NAME} 
      COMMAND ${TESTER_COMMAND} -m nose ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    set_tests_properties(${SOURCE_NAME} PROPERTIES LABELS "nose")
  endfunction()

  add_nose_test(test_visibility.py)
endif()
