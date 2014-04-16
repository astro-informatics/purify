# Creates an overall configuration file
configure_file(purify_config.in.h
    "${PROJECT_BINARY_DIR}/include/purify_config.h"
)

file(GLOB headers *.h)
install(FILES
    ${HEADER_FILES}
    "${PROJECT_BINARY_DIR}/include/purify_config." 
    DESTINATION include/purify COMPONENT DEV
)
