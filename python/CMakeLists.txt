if(python)
  include(TargetCopyFiles)

  set(libfftw3 ${FFTW3_DOUBLE_LIBRARY})
  set(libblas ${BLAS_LIBRARIES})
  set(libsopt ${Sopt_LIBRARIES})
  set(libcfitsio ${CFitsIO_LIBRARY})
  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  function(add_module TARGET)
      add_cython_modules(${TARGET}
          SOURCE purify.${TARGET}.pyx
          LOCATION ${PYPURIFY_DIRECTORY}
          LIBRARIES ${PYTHON_LIBRARIES} libpurify ${ARGN}
          DEPENDS copy
      )
      install_python(TARGETS py${TARGET} DESTINATION purify)
  endfunction()


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
  file(GLOB pysources *.py)

  add_custom_target(copy)

  add_module(visibility)
  add_module(image ${libcfitsio})
  add_module(sparse)
  add_module(measurements ${libblas} ${libfftw3})
  add_module(fftw ${libfftw3})
  add_module(sparsity_ops ${libsopt} ${libblas})
  add_module(sdmm  ${libsopt} ${libblas} ${libfftw3})
  add_module(rwsdmm  ${libsopt} ${libblas} ${libfftw3})

  add_copy_files(copy FILES ${pysources} DESTINATION ${PYPURIFY_DIRECTORY})
  if(tests)
    # Files only needed in testing. Added here cos here is where the copy target is defined.
    add_copy_files( copy GLOB "${PROJECT_SOURCE_DIR}/data/images/Coverages/*.vis"
                    DESTINATION "${PYPURIFY_DIRECTORY}/data/images/Coverages/" )
    add_copy_files( copy GLOB "${PROJECT_SOURCE_DIR}/data/images/*.fits"
                    DESTINATION "${PYPURIFY_DIRECTORY}/data/images/" )
    add_copy_files( copy GLOB "${PROJECT_SOURCE_DIR}/data/expected/*.fits"
                    DESTINATION "${PYPURIFY_DIRECTORY}/data/expected/" )
    add_copy_files( copy GLOB "${CMAKE_CURRENT_SOURCE_DIR}/tests/*fixtures.py"
                    DESTINATION "${PYPURIFY_DIRECTORY}/tests/" )
  endif()

  install_python(FILES *.pxd DESTINATION purify/headers)
  install_python(FILES ${pysources} DESTINATION purify)
endif()

add_subdirectory(tests)
