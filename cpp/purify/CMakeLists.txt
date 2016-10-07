# Add to install includes only if outside build dir
function(add_include_dir)
  unset(publics)
  unset(privates)
  foreach(dir ${ARGN})
    file(RELATIVE_PATH rel "${PROJECT_BINARY_DIR}" "${dir}" )
    if("${rel}" MATCHES "^\\.\\./")
      list(APPEND publics "${dir}")
    endif()
  endforeach() 
  if(public)
    target_include_directories(libpurify SYSTEM PUBLIC ${publics})
  endif()
endfunction()



configure_file(config.in.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(HEADERS 
  RMOperator.h logging.h FFTOperator.h kernels.h
  pfitsio.h MeasurementOperator.h clean.h logging.disabled.h types.h PSFOperator.h
  logging.enabled.h utilities.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(SOURCES MeasurementOperator.cc FFTOperator.cc clean.cc utilities.cc pfitsio.cc
  kernels.cc RMOperator.cc PSFOperator.cc)

if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc)
  list(APPEND HEADERS casacore.h)
endif()
add_library(libpurify SHARED ${SOURCES})
set(version "${Purify_VERSION_MAJOR}.${Purify_VERSION_MINOR}.${Purify_VERSION_PATCH}")
set_target_properties(libpurify PROPERTIES VERSION ${version} SOVERSION ${version})

target_include_directories(libpurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/cpp>
  $<INSTALL_INTERFACE:include/>)


add_include_dir(
  ${EIGEN3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${CFitsIO_INCLUDE_DIR}
  ${CCFits_INCLUDE_DIR}/..
)
target_link_libraries(libpurify
  ${FFTW3_DOUBLE_LIBRARY} ${CCFits_LIBRARY} ${CFitsIO_LIBRARY} ${Sopt_CPP_LIBRARY})
if(TARGET casacore::casa)
  target_link_libraries(libpurify casacore::ms)
endif()
if(TARGET openmp::openmp)
  target_link_libraries(libpurify openmp::openmp)
endif()

add_dependencies(libpurify lookup_dependencies)

install(FILES ${HEADERS} DESTINATION include/purify)
install(TARGETS libpurify
  EXPORT PurifyTargets
  DESTINATION share/cmake/Purify
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)
