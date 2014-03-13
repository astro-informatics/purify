if(python)
  include(TargetCopyFiles)

  set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

  add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_8_API_VERSION")

  include_directories(${PYTHON_INCLUDE_DIR} ${NUMPY_INCLUDE_DIRS} ${PYPURIFY_DIRECTORY}/include)

  ###############################
  # Add new targets
  ###############################

  add_custom_target(copy)
  foreach(module visibility image sparse measurements fftw sopt)
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

  # Copy pdx (cython's header files) to include directory on build dir
  # This hoop must be jumped if we want cython to find those headers, since compilation is from
  # files in the "python" directory, rather than a "purify" directory, as per normal python package
  # directory organisation. The alternative to having a python directory structure, is having pdx
  # files with names package.subpackage.pdx. This is what the next statements do.

  add_copy_files( copy GLOB *.pxd 
                  DESTINATION ${PYPURIFY_DIRECTORY}/include
                  REPLACE "([^ ]+)" "purify.\\1" )
endif()

add_subdirectory(tests)
