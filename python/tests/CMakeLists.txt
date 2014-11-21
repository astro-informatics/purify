if(tests AND python)
    add_pytest(test_*.py
        sparse_testing.pyx visibility_testing.pyx random.pyx
        image_fixtures.py
        DEPENDS copy
        FAKE_INIT
        PREFIX purify
        LIBRARIES libpurify ${Sopt_LIBRARIES}
    )
endif()
