if(python)
  include(TargetCopyFiles)

  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)
  # Add location of purify package in build tree,
  # so it can be found later on
  add_to_python_path(${CMAKE_CURRENT_BINARY_DIR})

  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_8_API_VERSION")

  include_directories(
      ${PYTHON_INCLUDE_DIR}
      ${NUMPY_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${BLAS_INCLUDE_DIR}
      ${FFTW3_INCLUDE_DIR}
      ${CFitsIO_INCLUDE_DIR}
      ${TIFF_INCLUDE_DIR}
      ${Sopt_INCLUDE_DIRS}
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
  target_link_libraries(pyfftw ${FFTW3_DOUBLE_LIBRARY})
  target_link_libraries(pyimage ${CFitsIO_LIBRARY})
  target_link_libraries(pymeasurements ${BLAS_LIBRARIES} ${FFTW3_DOUBLE_LIBRARY})
  target_link_libraries(pysdmm ${Sopt_LIBRARIES} ${BLAS_LIBRARIES} ${FFTW3_DOUBLE_LIBRARY})
  target_link_libraries(pysparsity_ops ${Sopt_LIBRARIES} ${BLAS_LIBRARIES})

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
