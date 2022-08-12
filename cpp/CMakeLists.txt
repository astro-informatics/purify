# adds include dirs which are located within the build directory
# That is, whathever package is downloaded automatically by the build system
# This complication arises because cmake won't install targets with headers that point to the build
# dir.
# Users will have to install packages globally for all the right headers to be found
function(add_include_dir)
  unset(privates)
  foreach(dir ${ARGN})
    file(RELATIVE_PATH rel "${PROJECT_BINARY_DIR}" "${dir}" )
    if(NOT "${rel}" MATCHES "^\\.\\./")
      list(APPEND privates "${dir}")
    endif()
  endforeach()
  if(privates)
    include_directories(SYSTEM ${privates})
  endif()
endfunction()

add_include_dir(
  ${Eigen3_INCLUDE_DIR}
  ${eigen_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${cfitsio_INCLUDE_DIR}
  ${yaml-cpp_INCLUDE_DIR}
  ${Cubature_INCLUDE_DIR}
  ${CImg_INCLUDE_DIR}
  )


add_subdirectory(purify)
if(tests OR examples OR benchmarks)
  configure_file(tests/directories.in.h "${PROJECT_BINARY_DIR}/include/purify/directories.h")
endif()
if(examples)
  add_subdirectory(example)
endif()
if(tests)
  configure_file(tests/data.in.h "${PROJECT_BINARY_DIR}/include/purify/test_data.h")
  add_subdirectory(tests)
endif()
if(benchmarks)
  add_subdirectory(benchmarks)
endif()
if (docs)
  add_subdirectory(docs)
endif()

add_executable(purify main.cc)
target_link_libraries(purify libpurify)
set_target_properties(purify PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

install(TARGETS purify
  EXPORT PurifyTargets
  DESTINATION share/cmake/Purify
  RUNTIME DESTINATION bin
  )

