
set(${PROJECT_NAME}_VERSION 3.1.0)

# Tries and get git hash first
find_package(Git)
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

set(${PROJECT_NAME}_GITREF ${GIT_HASH})

set(version ${Purify_VERSION})
string(REGEX REPLACE "\\." ";" version "${Purify_VERSION}")
list(GET version 0 Purify_VERSION_MAJOR)
list(GET version 1 Purify_VERSION_MINOR)
list(GET version 2 Purify_VERSION_PATCH)

