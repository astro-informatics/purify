set(EXAMPLE_SOURCES example_30dor.c example_ami.c example_m31.c example_m31_padmm.c example_realdata.c example_realdata_padmm.c)
set(PURIFY_SOURCES
  purify_measurement.c purify_error.c purify_ran.c purify_utils.c
  purify_image.c purify_sparsemat.c purify_visibility.c
)
set(TEST_SOURCE purify_test.c)
set(ABOUT_SOURCE purify_about.c)

# Creates library
add_library(libpurify SHARED ${PURIFY_SOURCES})
set_target_properties(libpurify PROPERTIES OUTPUT_NAME "purify")
# Adds include directories
if(BLAS_INCLUDE_DIR)
  target_include_directories(libpurify SYSTEM PUBLIC ${BLAS_INCLUDE_DIR})
endif()
target_include_directories(libpurify SYSTEM PUBLIC
  ${CFitsIO_INCLUDE_DIR}
  ${TIFF_INCLUDE_DIR}
)
target_include_directories(libpurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include/>)

target_link_libraries(libpurify
  ${Sopt_C_LIBRARY}
  ${FFTW3_DOUBLE_LIBRARY}
  ${CFitsIO_LIBRARY}
  ${TIFF_LIBRARY}
  ${BLAS_LIBRARIES}
)
depends_on_lookups(libpurify)

# Now add each example and purify_about
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
  # first extract name
  get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
  # then creates executable
  add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
  target_link_libraries(${EXAMPLE_NAME} libpurify)
endforeach()

# Add about
add_executable(purify_about ${ABOUT_SOURCE})
set_target_properties( purify_about PROPERTIES
  COMPILE_DEFINITIONS
    "PURIFY_VERSION=\"${${PROJECT_NAME}_VERSION}\";PURIFY_BUILD=\"${${PROJECT_NAME}_GITREF}\""
)
target_link_libraries(purify_about libpurify)


# add test
if(tests)
  add_executable(purify_test ${TEST_SOURCE})
  target_link_libraries(purify_test libpurify)
  create_environment_script(
      PATH "${CMAKE_CURRENT_BINARY_DIR}/purify_test.sh"
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  )
  add_test(
    NAME  purify
    COMMAND "${CMAKE_CURRENT_BINARY_DIR}/purify_test.sh"
        "$<TARGET_FILE:purify_test>"
  )
endif()

# install library and about
install(TARGETS libpurify DESTINATION ${LIBRARY_INSTALL_PATH})
if(NOT ${nobins})
    install(TARGETS purify_about DESTINATION bin)
endif()
