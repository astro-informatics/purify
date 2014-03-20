find_package(git)

# Set version variable
# If git is found and this is a git workdir, then figure out build id
# Stores results in ${PACKAGE_NAME}_VERSION and ${PACKAGE_NAME}_GITREF
function(version_and_build_id VERSION)

  # Tries and get git hash first
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      RESULT_VARIABLE HASH_RESULT
      OUTPUT_VARIABLE GIT_HASH
      ERROR_QUIET
    )
    if(HASH_RESULT EQUAL 0)
      string(STRIP "${GIT_HASH}" GIT_HASH)
    else()
      set(GIT_HASH "NA")
    endif()
  else()
    set(GIT_HASH "NA")
  endif()

  set(${PROJECT_NAME}_VERSION ${VERSION} PARENT_SCOPE)
  set(${PROJECT_NAME}_GITREF ${GIT_HASH} PARENT_SCOPE)

  if("${GIT_HASH}" STREQUAL "NA")
    message(STATUS "Building ${PROJECT_NAME} version ${VERSION}")
  else()
    message(STATUS "Building ${PROJECT_NAME} version ${VERSION}, with git reference ${GIT_HASH}")
  endif()
endfunction()
