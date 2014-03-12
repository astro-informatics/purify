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
    add_test(NAME ${testname} COMMAND ${TESTER_COMMAND} -m nose ${SOURCE})
    set_tests_properties(${testname} PROPERTIES LABELS "nose")
  endfunction()

  # Copy pdx (cython's header files) to include directory on build dir
  # This hoop must be jumped if we want cython to find those headers, since compilation is from
  # files in the "python" directory, rather than a "purify" directory, as per normal python package
  # directory organisation. The alternative to having a python directory structure, is having pdx
  # files with names package.subpackage.pdx. This is what the next statements do.
  add_custom_target(test_copy)
  include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

  add_copy_files( test_copy GLOB ${PROJECT_SOURCE_DIR}/python/*.pxd 
                  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include
                  REPLACE "([^ ]+)" "purify.\\1" )

  # add testing packages
  add_cython_modules(sparse_testing
    LOCATION ${PYPURIFY_DIRECTORY}/tests
    LIBRARIES ${PYTHON_LIBRARIES} 
    DEPENDS test_copy
  )

  # Add fake __init__.py file so tests can import the test packages introduced above
  file(WRITE ${PYPURIFY_DIRECTORY}/tests/__init__.py "")


  # Now add tests 
  add_nose_test(visibility)
  add_nose_test(image)
  add_nose_test(sparse)
endif()
