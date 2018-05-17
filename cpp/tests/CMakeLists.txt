if(logging)
  add_library(common_catch_main_object OBJECT "common_catch_main.cc")
  if(SPDLOG_INCLUDE_DIR)
    target_include_directories(common_catch_main_object SYSTEM PUBLIC ${SPDLOG_INCLUDE_DIR})
  endif()
  if(CATCH_INCLUDE_DIR)
    target_include_directories(common_catch_main_object SYSTEM PUBLIC ${CATCH_INCLUDE_DIR})
  endif()
  if(Sopt_INCLUDE_DIRS)
    target_include_directories(common_catch_main_object SYSTEM PUBLIC ${Sopt_INCLUDE_DIRS})
  endif()
  target_include_directories(common_catch_main_object PUBLIC
    "${PROJECT_BINARY_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    )
  add_dependencies(common_catch_main_object lookup_dependencies)
endif()
include_directories("${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/outputs")

add_catch_test(kernels LIBRARIES libpurify)
add_catch_test(measurement_operator LIBRARIES libpurify)
add_catch_test(purify_fitsio LIBRARIES libpurify)
add_catch_test(distribute LIBRARIES libpurify)
add_catch_test(utils LIBRARIES libpurify)
add_catch_test(operators LIBRARIES libpurify)
add_catch_test(sparse LIBRARIES libpurify)
add_catch_test(index_mapping LIBRARIES libpurify)
add_catch_test(uvfits LIBRARIES libpurify)
add_catch_test(convolution LIBRARIES libpurify)
add_catch_test(wproj_utils LIBRARIES libpurify)
add_catch_test(measurement_factory LIBRARIES libpurify)
add_catch_test(wavelet_factory LIBRARIES libpurify)

if(data AND TARGET casacore::ms)
  add_catch_test(casacore LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()

if(PURIFY_ARRAYFIRE)
  include_directories(${ArrayFire_INCLUDE_DIRS})
  add_catch_test(operators_gpu LIBRARIES libpurify ${ArrayFire_LIBRARIES})
endif()

if(PURIFY_MPI)
  add_library(common_mpi_catch_main_object OBJECT common_mpi_catch_main.cc)
  target_include_directories(common_mpi_catch_main_object
    PUBLIC ${Sopt_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/cpp
    ${PROJECT_BINARY_DIR}/include ${MPI_CXX_INCLUDE_PATH})
  add_dependencies(common_mpi_catch_main_object lookup_dependencies)

  function(add_mpi_test testname)
    add_catch_test(${testname} COMMON_MAIN common_mpi_catch_main_object NOTEST ${ARGN})
    unset(arguments)
    if(CATCH_JUNIT)
      set(arguments -r junit -o ${PROJECT_BINARY_DIR}/Testing/${testname}.xml)
    endif()
    if(NOT MPIEXEC_MAX_NUMPROCS)
      set(MPIEXEC_MAX_NUMPROCS 4)
    endif()
    add_test(NAME ${testname}
      COMMAND
      ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS}
      test_${testname} ${arguments})
    set_tests_properties(${testname} PROPERTIES LABELS "catch;mpi")
  endfunction()

  add_catch_test(mpi_utilities LIBRARIES libpurify)
  add_mpi_test(parallel_mpi_utilities LIBRARIES libpurify)
  add_mpi_test(serial_vs_parallel_padmm LIBRARIES libpurify)
  add_mpi_test(mpi_measurement_operator LIBRARIES libpurify)
  add_mpi_test(mpi_measurement_factory LIBRARIES libpurify)
  add_mpi_test(mpi_wavelet_factory LIBRARIES libpurify)
  add_mpi_test(distribute_sparse_vector LIBRARIES libpurify)
endif()
