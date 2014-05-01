# Creates an overall configuration file
configure_file(purify_config.in.h
    "${PROJECT_BINARY_DIR}/include/purify_config.h"
)

file(GLOB headers *.h)
list(REMOVE_ITEM headers "${CMAKE_CURRENT_SOURCE_DIR}/purify_config.in.h")

if(NOT dont_install_headers)
    install(FILES
        ${headers}
        "${PROJECT_BINARY_DIR}/include/purify_config.h"
        DESTINATION include/purify COMPONENT DEV
    )
endif()
