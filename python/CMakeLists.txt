if(python)
  include(TargetCopyFiles)

  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS})
  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")
  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  ###############################
  # Add new targets
  ###############################

  add_custom_target(copy)
  foreach(module visibility image sparse)
    add_cython_modules(${module}
      LOCATION ${PYPURIFY_DIRECTORY}
      LIBRARIES ${PYTHON_LIBRARIES} libpurify
      DEPENDS copy
      INSTALL_DESTINATION ${PYTHON_PKG_DIR}/purify
    )
  endforeach()

  add_copy_files(copy GLOB *.py DESTINATION ${PYPURIFY_DIRECTORY})
  add_copy_files(copy GLOB *.pdx DESTINATION ${PYPURIFY_DIRECTORY})
  add_copy_files( copy GLOB ${PROJECT_SOURCE_DIR}/data/images/Coverages/*.vis 
                  DESTINATION ${PYPURIFY_DIRECTORY}/data/images/Coverages/ )
  add_copy_files( copy GLOB ${PROJECT_SOURCE_DIR}/data/images/*.fits
                  DESTINATION ${PYPURIFY_DIRECTORY}/data/images/ )

endif()

add_subdirectory(tests)
