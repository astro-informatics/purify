include(PythonPackage)
# Adds a python module as a target
function(add_python_module NAME)
  cmake_parse_arguments(PYMODULE
      "" "LOCATION" "DEPENDS;LIBRARIES;FILES" ${ARGN})

  set(module_name py${NAME})

  if(NOT PYMODULE_FILES)
    set(sources ${PYMODULE_UNPARSED_ARGUMENTS})
  else()
    set(sources ${PYMODULE_FILES})
  endif()

  add_library (${module_name} SHARED ${sources})
  set_target_properties(${module_name} PROPERTIES OUTPUT_NAME ${NAME})
  set_target_properties(${module_name} PROPERTIES PREFIX "")
  set_target_properties(${module_name} PROPERTIES SUFFIX ".so")

  target_link_libraries(${module_name} ${PYTHON_LIBRARIES})
  if(NOT "${PYMODULE_LIBRARIES}" STREQUAL "")
    target_link_libraries(${module_name} ${PYMODULE_LIBRARIES})
  endif()
  if(NOT "${PYMODULE_DEPENDS}" STREQUAL "")
    add_dependencies(${module_name} ${PYMODULE_DEPENDS})
  endif()
  if(NOT "${PYMODULE_LOCATION}" STREQUAL "")
    set_target_properties(${module_name}
      PROPERTIES LIBRARY_OUTPUT_DIRECTORY
      ${PYMODULE_LOCATION}
    )
  endif()
endfunction()

set(FIND_DEPS_SCRIPT
    ${CMAKE_CURRENT_LIST_DIR}/find_cython_deps.py
    CACHE INTERNAL "Script to determine cython dependencies"
)
# Get dependencies from a cython file
function(get_pyx_dependencies SOURCE OUTVAR)
  _python_executable(LOCALPYTHON)
  execute_process(
      COMMAND ${LOCALPYTHON} ${FIND_DEPS_SCRIPT} ${SOURCE} ${ARGN}
      RESULT_VARIABLE RESULT
      OUTPUT_VARIABLE OUTPUT
      ERROR_VARIABLE ERROR
  )
  if("${RESULT}" STREQUAL "0")
      set(${OUTVAR} ${OUTPUT} PARENT_SCOPE)
  else()
      message("Error: ${ERROR}")
      message("Output: ${OUTPUT}")
      message(FATAL_ERROR "Error while computing cython dependencies")
  endif()
endfunction()



# Creates a cython mpdule from an input file
function(add_cython_modules TARGET)

  _python_executable(LOCALPYTHON ${ARGN})
  cmake_parse_arguments(CYMODULE "" "SOURCE" "" ${LOCALPYTHON_UNPARSED_ARGUMENTS})

  set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  if(NOT CYMODULE_SOURCE)
      set(CYMODULE_SOURCE ${TARGET}.pyx)
  endif()

  get_property(included_dirs
      DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      PROPERTY INCLUDE_DIRECTORIES
  )
  set(inclusion)
  foreach(included ${included_dirs})
    set(inclusion ${inclusion} -I${included})
  endforeach()

  get_pyx_dependencies(${CMAKE_CURRENT_SOURCE_DIR}/${CYMODULE_SOURCE} DEPENDENCIES ${included_dirs})

  if(cython_EXECUTABLE)
      add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${cython_EXECUTABLE} ${CYMODULE_SOURCE} -o ${OUTPUT_FILE} ${inclusion}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${DEPENDENCIES}
      )
  else()
      add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${LOCALPYTHON} -m cython ${CYMODULE_SOURCE}
                               -o ${OUTPUT_FILE} ${inclusion}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${DEPENDENCIES}
      )
  endif()

  add_python_module(${TARGET} ${CYMODULE_UNPARSED_ARGUMENTS} FILES ${OUTPUT_FILE})
  target_link_libraries(py${TARGET} ${TARGET_LIBRARIES})

endfunction()

find_package(PythonInterp REQUIRED)
get_filename_component(directory "${PYTHON_EXECUTABLE}" PATH)
find_program(cython_EXECUTABLE HINTS "${directory}")
