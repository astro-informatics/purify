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
    add_nose_test(test_visibility.py)
    add_nose_test(test_image.py)
    add_nose_test(test_sparse.py)
    add_nose_test(test_fftw.py)
    add_nose_test(test_sparsity_ops.py)
    add_nose_test(test_params.py)
    add_nose_test(test_image_fixtures.py)
    add_nose_test(test_sdmm.py)
endif()
