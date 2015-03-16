if(NOT python)
    return()
endif()

include(TargetCopyFiles)
include(PythonModule)

set(PYPURIFY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/purify)

# Add location of purify package in build tree,
# so it can be found later on
add_to_python_path(${CMAKE_CURRENT_BINARY_DIR})

add_definitions("-DNPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION")

include_directories(
    ${PYTHON_INCLUDE_DIR}
    ${NUMPY_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FFTW3_INCLUDE_DIR}
    ${CFitsIO_INCLUDE_DIR}
    ${TIFF_INCLUDE_DIR}
    ${Sopt_INCLUDE_DIRS}
)
if(BLAS_INCLUDE_DIR)
    include_directories(${BLAS_INCLUDE_DIR})
endif()

###############################
# Add new targets
###############################
file(GLOB PXD_HEADERS *.pxd)
file(GLOB PYTHON_SOURCES *.py)
list(REMOVE_ITEM PYTHON_SOURCES task_purify_image.py)

add_custom_target(copy ALL)
add_python_module(purify
    purify.visibility.pyx purify.image.pyx purify.sparse.pyx purify.sensing.pyx
    purify.fftw.pyx purify.sparsity_ops.pyx purify.sdmm.pyx purify.cparams.pyx
    ${PYTHON_SOURCES} ${PXD_HEADERS}
    LIBRARIES libpurify
)
if(tests)
    # Files only needed in testing. Added here cos here is where the copy
    # target is defined.
    add_copy_directory(copy
        "${PROJECT_SOURCE_DIR}/data/"
        GLOB *.vis *.fits
        DESTINATION "${PYTHON_BINARY_DIR}/purify/data"
    )
    install_python(
        DIRECTORY "${PROJECT_SOURCE_DIR}/data/"
        DESTINATION ${PYTHON_PKG_DIR}/purify/data
        FILES_MATCHING
        PATTERN "images/Coverages/*.vis"
        PATTERN "images/*.fits"
        PATTERN "expected/*.fits"
        PATTERN "test/*" EXCLUDE
    )
endif()

add_copy_files(copy
    FILES purify_image.xml task_purify_image.py
    DESTINATION ${PROJECT_BINARY_DIR}/casa_task
)
add_subdirectory(tests)
