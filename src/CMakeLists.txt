set(EXAMPLE_SOURCES example_30dor.c example_ami.c example_m31.c)
set(PURIFY_SOURCES
  purify_measurement.c purify_error.c purify_ran.c purify_utils.c purify_image.c
  purify_sparsemat.c purify_visibility.c
)
set(TEST_SOURCE purify_test.c)
set(ABOUT_SOURCE purify_about.c)


# Creates library
add_library(libpurify ${PURIFY_SOURCES})

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
