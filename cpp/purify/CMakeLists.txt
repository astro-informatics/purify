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
  if(publics)
    target_include_directories(libpurify SYSTEM PUBLIC ${publics})
  endif()
endfunction()



configure_file(config.in.h "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(HEADERS
  logging.h kernels.h pfitsio.h  logging.disabled.h types.h
  IndexMapping.h logging.enabled.h utilities.h operators.h  wproj_utilities.h
  cimg.h uvfits.h convolution.h measurement_operator_factory.h wavelet_operator_factory.h distribute.h
  update_factory.h
  convergence_factory.h
  algorithm_factory.h
  read_measurements.h
  yaml-parser.h
  integration.h
  wide_field_utilities.h
  wkernel_integration.h
  wproj_operators.h
  uvw_utilities.h
  fly_operators.h
  "${PROJECT_BINARY_DIR}/include/purify/config.h")

set(SOURCES utilities.cc pfitsio.cc
  kernels.cc wproj_utilities.cc operators.cc uvfits.cc yaml-parser.cc
  read_measurements.cc distribute.cc integration.cc wide_field_utilities.cc wkernel_integration.cc
  wproj_operators.cc uvw_utilities.cc)

if(TARGET casacore::ms)
  list(APPEND SOURCES casacore.cc)
  list(APPEND HEADERS casacore.h)
endif()

if(PURIFY_MPI)
  list(APPEND HEADERS mpi_utilities.h distribute.h DistributeSparseVector.h
  random_update_factory.h
  AllToAllSparseVector.h)
  list(APPEND SOURCES mpi_utilities.cc distribute.cc AllToAllSparseVector.cc
  random_update_factory.cc)
endif()

add_library(libpurify SHARED ${SOURCES})
set(version "${Purify_VERSION_MAJOR}.${Purify_VERSION_MINOR}.${Purify_VERSION_PATCH}")
set(soversion "${Purify_VERSION_MAJOR}.${Purify_VERSION_MINOR}")
set_target_properties(libpurify PROPERTIES VERSION ${version} SOVERSION ${soversion})
set_target_properties(libpurify PROPERTIES OUTPUT_NAME purify)
if(PURIFY_MPI)
  target_link_libraries(libpurify ${MPI_LIBRARIES})
  target_include_directories(libpurify SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
endif()

if(PURIFY_ARRAYFIRE)
  list(APPEND HEADERS operators_gpu.h wproj_operators_gpu.h)
  target_link_libraries(libpurify ${ArrayFire_LIBRARIES})
endif()

target_include_directories(libpurify PUBLIC
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/cpp>
  $<INSTALL_INTERFACE:include/>)


add_include_dir(
  ${EIGEN3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${CFitsIO_INCLUDE_DIR}
  ${Yamlcpp_INCLUDE_DIR}
  ${Cubature_INCLUDE_DIR}
  ${CImg_INCLUDE_DIR}
)
target_link_libraries(libpurify
  ${FFTW3_DOUBLE_LIBRARY}  ${CFitsIO_LIBRARY} ${Sopt_CPP_LIBRARY} ${X11_X11_LIB} ${Yamlcpp_LIBRARY} ${Cubature_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
  )
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
