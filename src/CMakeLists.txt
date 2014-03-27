set(EXAMPLE_SOURCES example_30dor.c example_ami.c example_m31.c)
set(PURIFY_SOURCES
  purify_measurement.c purify_error.c purify_ran.c purify_utils.c purify_image.c
  purify_sparsemat.c purify_visibility.c
)
set(TEST_SOURCE purify_test.c)
set(ABOUT_SOURCE purify_about.c)

# Adds include directories
include_directories(
  ${BLAS_INCLUDE_DIR}
  ${FFTW3_INCLUDE_DIR}
  ${CFitsIO_INCLUDE_DIR}
  ${TIFF_INCLUDE_DIR}
  ${Sopt_INCLUDE_DIRS}
)

# Target libraries for the different executables
set(TARGET_LIBRARIES
     libpurify
     ${Sopt_LIBRARIES}
     ${FFTW3_DOUBLE_LIBRARY}
     ${CFitsIO_LIBRARY}
     ${TIFF_LIBRARY}
     ${BLAS_LIBRARIES}
)

# Creates library
add_library(libpurify ${PURIFY_SOURCES})
set_target_properties(libpurify PROPERTIES OUTPUT_NAME "purify")
depends_on_lookups(libpurify)

# Now add each example and purify_about
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
  # first extract name
  get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
  # then creates executable
  add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
  target_link_libraries(${EXAMPLE_NAME} ${TARGET_LIBRARIES})
endforeach()

# Add about
add_executable(purify_about ${ABOUT_SOURCE})
set_target_properties( purify_about PROPERTIES
  COMPILE_DEFINITIONS
    "PURIFY_VERSION=\"${${PROJECT_NAME}_VERSION}\";PURIFY_BUILD=\"${${PROJECT_NAME}_GITREF}\""
)
target_link_libraries(purify_about ${TARGET_LIBRARIES})


# add test
if(tests)
  add_executable(purify_test ${TEST_SOURCE})
  target_link_libraries(purify_test ${TARGET_LIBRARIES})
  add_test(
    NAME purify
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/purify_test
  )
endif()

# install library and about
install(TARGETS libpurify DESTINATION lib)
install(TARGETS purify_about DESTINATION bin)
