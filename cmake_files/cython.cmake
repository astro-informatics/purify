# Adds some cython related functions

function(relative_path PATH BASE OUTVAR)
  execute_process(
    COMMAND
      ${PYTHON_EXECUTABLE} -c "from os.path import relpath; print(relpath('${PATH}', '${BASE}'))"
    OUTPUT_VARIABLE OUTVAR
  )
  set(${OUTVAR} ${${OUTVAR}} PARENT_SCOPE)
endfunction()


# Adds a python module as a target
function(add_python_module NAME)
  set(oneValueArgs LOCATION INSTALL_DESTINATION)
  set(multiValueArgs DEPENDS LIBRARIES FILES)
  cmake_parse_arguments(PYMODULE "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  set(module_name py${NAME})

  if(${PYMODULE_FILES} STREQUAL "")
    set(sources ${ARGN})
  else()
    set(sources ${PYMODULE_FILES})
  endif()

  add_library (${module_name} SHARED ${sources})
  set_target_properties(${module_name} PROPERTIES OUTPUT_NAME ${NAME})
  set_target_properties(${module_name} PROPERTIES PREFIX "")
  set_target_properties(${module_name} PROPERTIES SUFFIX ".so")

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
  if(NOT ${PYMODULE_INSTALL_DESTINATION} STREQUAL "")
    install(TARGETS ${module_name} DESTINATION ${PYMODULE_INSTALL_DESTINATION})
  endif()
endfunction()

# Get dependencies from a cython file
function(get_pyx_dependencies NAME OUTVAR)
  set(${OUTVAR} ${ARGN})
  foreach(filename ${NAME}.pyx ${NAME}.pxd)
    set(filename ${CMAKE_CURRENT_SOURCE_DIR}/${filename})
    if(EXISTS ${filename})
      set(${OUTVAR} ${${OUTVAR}} ${filename})
    endif()
  endforeach()
  set(${OUTVAR} ${${OUTVAR}} PARENT_SCOPE)
endfunction()



# Creates a cython mpdule from an input file
function(add_cython_modules TARGET)

  set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.c)
  get_pyx_dependencies(${TARGET} DEPENDENCIES)

  get_property(included_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
  set(inclusion)
  foreach(included ${included_dirs})
    set(inclusion ${inclusion} -I${included})
  endforeach()

  add_custom_command(
    OUTPUT ${OUTPUT_FILE}
    COMMAND ${PYTHON_EXECUTABLE} -m cython ${TARGET}.pyx -o ${OUTPUT_FILE} ${inclusion}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DEPENDENCIES}
  )

  add_python_module(${TARGET} ${ARGN} FILES ${OUTPUT_FILE})
  add_dependencies(py${TARGET} ${DEPENDENCIES})
  target_link_libraries(py${TARGET} ${TARGET_LIBRARIES})

endfunction()
