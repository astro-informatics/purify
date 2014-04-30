if(tests AND python)
    # A function to add tests
    include(nose)

    # add testing packages
    foreach(module sparse_testing visibility_testing random)
        add_cython_modules(${module}
          LOCATION ${PYPURIFY_DIRECTORY}/tests
          LIBRARIES ${PYTHON_LIBRARIES}
          DEPENDS copy
        )
    endforeach()
    target_link_libraries(pyrandom ${Sopt_LIBRARIES})
    depends_on_lookups(pyrandom)
    target_link_libraries(pysparse_testing libpurify)

    # Add fake __init__.py file so tests can import the test packages introduced above
    file(WRITE "${PYPURIFY_DIRECTORY}/tests/__init__.py"
        "# makes this directory a sub-module"
    )


    # Now add tests
    file(GLOB tests test_*.py)
    foreach(test ${tests})
        get_filename_component(test "${test}" NAME)
        add_nose_test(${test})
    endforeach()

    file(GLOB fixtures *fixtures.py)
    install_python(
        FILES ${fixtures} ${tests} "${PYPURIFY_DIRECTORY}/tests/__init__.py"
        DESTINATION purify/tests
    )
    install_python(
        TARGETS pyrandom pysparse_testing pyvisibility_testing
        DESTINATION purify/tests
    )
endif()
