include_directories("${PROJECT_SOURCE_DIR}/cpp" "${CMAKE_CURRENT_BINARY_DIR}/include/purify")
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/outputs")
add_catch_test(measurement_operator LIBRARIES libpurify)
add_catch_test(FFT_operator LIBRARIES libpurify)
add_catch_test(purify_fitsio LIBRARIES libpurify)
add_catch_test(distribute LIBRARIES libpurify)
add_catch_test(utils LIBRARIES libpurify)
add_catch_test(sparse LIBRARIES libpurify)
if(data AND TARGET casacore::ms)
  add_catch_test(casacore LIBRARIES libpurify ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()

if(PURIFY_MPI)
  add_library(common_mpi_catch_main_object OBJECT common_mpi_catch_main.cc)
  target_include_directories(common_mpi_catch_main_object
    PUBLIC ${Sopt_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/cpp
           ${PROJECT_BINARY_DIR}/include ${MPI_CXX_INCLUDE_PATH})

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
endif()
