if(python)
  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS})
  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")
  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  ########################################
  # Define new functions to create targets
  ########################################

  # Creates a python module from a C input file
  function(add_python_module NAME) 
    add_library (py${NAME} SHARED ${ARGN})
    set_target_properties(py${NAME} PROPERTIES OUTPUT_NAME ${NAME})
    set_target_properties(py${NAME} PROPERTIES PREFIX "")
    set_target_properties(py${NAME} PROPERTIES SUFFIX ".so")
    target_link_libraries(py${NAME} ${PYTHON_LIBRARIES} libpurify)
    # Change path to something such that we can import it in tests.
    set_target_properties(py${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYPURIFY_DIRECTORY})

  endfunction()

  # Creates a cython C source file from a pyx input file
  function(add_cython_modules TARGET)

    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
    add_custom_command(
      OUTPUT ${OUTPUT_FILE}
      COMMAND ${PYTHON_EXECUTABLE} -m cython ${TARGET}.pyx -o ${OUTPUT_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${TARGET}.pyx
    )
  
    add_python_module(${TARGET} ${OUTPUT_FILE})
    add_dependencies(py${TARGET} ${TARGET}.pyx)
    target_link_libraries(py${TARGET} ${TARGET_LIBRARIES})
    install(TARGETS py${TARGET} DESTINATION ${PYTHON_PKG_DIR}/purify)
    add_dependencies(pyvisibility python_files)

  endfunction()

  # Copy python files to output package
  function(add_python_files THEGLOB OUTPUT_DIR)
    file(GLOB input_sources ${THEGLOB})
    foreach(input ${input_sources})
      get_filename_component(output ${input} NAME)
      set(output ${OUTPUT_DIR}/${output})
      add_custom_command(
        TARGET python_files
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${input} ${output}
        DEPENDS ${input}
      )
    endforeach()
  endfunction() 
  

  ###############################
  # Add new targets
  ###############################

  add_custom_target(python_files)
  add_cython_modules(visibility)
  add_cython_modules(image)
  add_cython_modules(griding)

  add_python_files(*.py ${PYPURIFY_DIRECTORY})
  add_python_files( ${PROJECT_SOURCE_DIR}/data/images/Coverages/*.vis 
                    ${PYPURIFY_DIRECTORY}/data/images/Coverages/ )
  add_python_files( ${PROJECT_SOURCE_DIR}/data/images/*.fits
                    ${PYPURIFY_DIRECTORY}/data/images/ )

endif()

add_subdirectory(tests)
