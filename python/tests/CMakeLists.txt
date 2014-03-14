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

  # A function to add tests
  function(add_nose_test SOURCE) 
    get_filename_component(testname ${SOURCE} NAME_WE)
    add_test(NAME ${testname} COMMAND ${TESTER_COMMAND} -m nose ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE})
    set_tests_properties(${testname} PROPERTIES LABELS "nose")
  endfunction()

  # add testing packages
  add_cython_modules(sparse_testing
    LOCATION ${PYPURIFY_DIRECTORY}/tests
    LIBRARIES ${PYTHON_LIBRARIES}
    DEPENDS test_copy
  )
  add_cython_modules(visibility_testing
    LOCATION ${PYPURIFY_DIRECTORY}/tests
    LIBRARIES ${PYTHON_LIBRARIES}
    DEPENDS test_copy
  )

  # Add fake __init__.py file so tests can import the test packages introduced above
  file(WRITE ${PYPURIFY_DIRECTORY}/tests/__init__.py "")


  # Now add tests 
  add_nose_test(test_visibility.py)
  add_nose_test(test_image.py)
  add_nose_test(test_sparse.py)
  add_nose_test(test_fftw.py)
  add_nose_test(test_sparsity_ops.py)
endif()
