if(python)
  include(TargetCopyFiles)

  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_8_API_VERSION")

  include_directories(
      ${PYTHON_INCLUDE_DIR}
      ${NUMPY_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}
  )

  ###############################
  # Add new targets
  ###############################
  file(GLOB PXD_HEADERS *.pxd)
  file(GLOB PYTHON_SOURCES *.py)
  set(cython_modules visibility image sparse measurements fftw sparsity_ops sdmm)


  add_custom_target(copy)
  set(python_modules)
  foreach(module ${cython_modules})
    add_cython_modules(${module}
      SOURCE purify.${module}.pyx
      LOCATION ${PYPURIFY_DIRECTORY}
      LIBRARIES ${PYTHON_LIBRARIES} libpurify
      DEPENDS copy
    )
    list(APPEND python_modules py${module})
  endforeach()

  add_copy_files(copy FILES ${PYTHON_SOURCES} DESTINATION ${PYPURIFY_DIRECTORY})
  if(tests)
    # Files only needed in testing. Added here cos here is where the copy target is defined.
    add_copy_files( copy GLOB ${PROJECT_SOURCE_DIR}/data/images/Coverages/*.vis
                    DESTINATION ${PYPURIFY_DIRECTORY}/data/images/Coverages/ )
    add_copy_files( copy GLOB ${PROJECT_SOURCE_DIR}/data/images/*.fits
                    DESTINATION ${PYPURIFY_DIRECTORY}/data/images/ )
    add_copy_files( copy GLOB ${PROJECT_SOURCE_DIR}/data/expected/*.fits
                    DESTINATION ${PYPURIFY_DIRECTORY}/data/expected/ )
  endif()

  install_python(TARGETS ${python_modules} DESTINATION purify)
  install_python(FILES ${PXD_HEADERS} DESTINATION purify/headers)
  install_python(FILES ${PYTHON_SOURCES} DESTINATION purify)
endif()

add_subdirectory(tests)
