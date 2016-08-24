# Exports Purify so other packages can access it
export(TARGETS libpurify purify FILE "${PROJECT_BINARY_DIR}/PurifyTargets.cmake")

# Avoids creating an entry in the cmake registry.
if(NOT NOEXPORT)
    export(PACKAGE Purify)
endif()

# First in binary dir
set(ALL_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/cpp" "${PROJECT_BINARY_DIR}/include")
configure_File(cmake_files/PurifyConfig.in.cmake
    "${PROJECT_BINARY_DIR}/PurifyConfig.cmake" @ONLY
)
configure_File(cmake_files/PurifyConfigVersion.in.cmake
    "${PROJECT_BINARY_DIR}/PurifyConfigVersion.cmake" @ONLY
)

# Then for installation tree
file(RELATIVE_PATH REL_INCLUDE_DIR
    "${CMAKE_INSTALL_PREFIX}/share/cmake/purify"
    "${CMAKE_INSTALL_PREFIX}/include"
)
set(ALL_INCLUDE_DIRS "\${Purify_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake_files/PurifyConfig.in.cmake
    "${PROJECT_BINARY_DIR}/CMakeFiles/PurifyConfig.cmake" @ONLY
)

# Finally install all files
install(FILES
    "${PROJECT_BINARY_DIR}/CMakeFiles/PurifyConfig.cmake"
    "${PROJECT_BINARY_DIR}/PurifyConfigVersion.cmake"
    DESTINATION share/cmake/purify
    COMPONENT dev
)

install(EXPORT PurifyTargets DESTINATION share/cmake/purify COMPONENT dev)
