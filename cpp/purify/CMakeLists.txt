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
  logging.h kernels.h pfitsio.h types.h
  IndexMapping.h utilities.h operators.h wproj_utilities.h
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

set(SOURCES utilities.cc pfitsio.cc logging.cc
  kernels.cc wproj_utilities.cc operators.cc uvfits.cc yaml-parser.cc
  read_measurements.cc distribute.cc integration.cc wide_field_utilities.cc wkernel_integration.cc
  wproj_operators.cc uvw_utilities.cc)

if(PURIFY_CASACORE)
  list(APPEND SOURCES casacore.cc)
  list(APPEND HEADERS casacore.h)
endif()

if(PURIFY_H5)
  list(APPEND HEADERS h5reader.h)
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

if(PURIFY_OPENMP)
  target_link_libraries(libpurify OpenMP::OpenMP_CXX)
endif()

if(PURIFY_MPI)
  target_link_libraries(libpurify ${MPI_LIBRARIES})
  target_include_directories(libpurify SYSTEM PUBLIC ${MPI_CXX_INCLUDE_PATH})
endif()

if(PURIFY_ARRAYFIRE)
  list(APPEND HEADERS operators_gpu.h wproj_operators_gpu.h)
  target_link_libraries(libpurify ${ArrayFire_LIBRARIES})
endif()

if(PURIFY_H5)
  target_link_libraries(libpurify ${HDF5_CXX_LIBRARIES})
  target_include_directories(libpurify SYSTEM PUBLIC ${HDF5_INCLUDE_DIR} ${HighFive_INCLUDE_DIR})
endif()

target_include_directories(libpurify PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include/>)

add_include_dir(
  ${EIGEN3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${cfitsio_INCLUDE_DIR}
  ${FFTW3_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIR}
  ${Cubature_INCLUDE_DIR}
  ${CImg_INCLUDE_DIR}
)

target_link_libraries(libpurify
  ${Cubature_LIBRARIES}
  ${FFTW3_LIBRARIES}
  cfitsio
  ${sopt_LIBRARIES}
  yaml-cpp
  Boost::filesystem
  Boost::system
)

if(PURIFY_CASACORE)
  target_link_libraries(libpurify ${CasaCore_LIBRARIES})
endif()

if(TARGET openmp::openmp)
  target_link_libraries(libpurify openmp::openmp)
endif()

if(PURIFY_CUBATURE_LOOKUP)
  add_dependencies(libpurify Lookup-Cubature)
endif()

if(PURIFY_CASACORE_LOOKUP)
  add_dependencies(libpurify Lookup-CasaCore)
endif()

if(PURIFY_HIGHFIVE_LOOKUP)
  add_dependencies(libpurify Lookup-HighFive)
endif()

install(FILES ${HEADERS} DESTINATION include/purify)
install(TARGETS libpurify
  EXPORT PurifyTargets
  DESTINATION share/cmake/Purify
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)
