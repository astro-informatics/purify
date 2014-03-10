if(python)
  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS})
  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")
  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  function(add_python_module NAME) 
    add_library (py${NAME} SHARED ${ARGN})
    set_target_properties(py${NAME} PROPERTIES OUTPUT_NAME ${NAME})
    set_target_properties(py${NAME} PROPERTIES PREFIX "")
    set_target_properties(py${NAME} PROPERTIES SUFFIX ".so")
    target_link_libraries(py${NAME} ${PYTHON_LIBRARIES} libpurify)
    # Change path to something such that we can import it in tests.
    set_target_properties(py${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYPURIFY_DIRECTORY})

  endfunction()

  function(add_cython_module TARGET)

    set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
    add_custom_command(
      OUTPUT ${OUTPUT_FILE}
      COMMAND ${PYTHON_EXECUTABLE} -m cython ${ARGN} -o ${OUTPUT_FILE}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${ARGN}
    )

    add_python_module(${TARGET} ${OUTPUT_FILE})
    add_dependencies(py${TARGET} ${ARGN})

  endfunction()

  add_cython_module(visibility visibility.pyx)

  # Copy python files to output package
  add_custom_target(python_files)
  function(add_python_files THEGLOB OUTPUT_DIR)
    file(GLOB input_sources ${THEGLOB})
    foreach(input ${input_sources})
      add_custom_command(
        TARGET python_files
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${input} ${OUTPUT_DIR}
        DEPENDS ${input}
      )
      add_dependencies(pyvisibility python_files)
    endforeach()
  endfunction() 

  add_python_files(*.py ${PYPURIFY_DIRECTORY})
  add_python_files( ${PROJECT_SOURCE_DIR}/data/images/Coverages/*.vis 
                    ${PYPURIFY_DIRECTORY}/data/images/Coverage/ )

  # Installation
  install(TARGETS pyvisibility DESTINATION ${PYTHON_PKG_DIR}/purify)
endif()

add_subdirectory(tests)
