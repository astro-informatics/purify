# Download ngc3256 data from the web
set(NGC3256_TGZ "${PROJECT_BINARY_DIR}/data/ngc3526.tgz")
set(NGC3256_MS "${PROJECT_BINARY_DIR}/data/NGC3256_Band3_CalibratedData/ngc3256_line_target.ms")
if(NOT EXISTS "${NGC3256_TGZ}")
  message(STATUS "Downloading ngc3256 data for testing: disable this with -Ddata=OFF")
endif()
file(DOWNLOAD
  https://almascience.eso.org/almadata/sciver/NGC3256/NGC3256_Band3_CalibratedData_CASA3.3.tgz
  ${NGC3256_TGZ}
  EXPECTED_HASH MD5=af0f4547b542f75e2476dfc437d991ce
)

# untar the data
if(EXISTS "${NGC3256_TGZ}" AND NOT EXISTS "${NGC3256_MS}")
  find_program(TAR_PROGRAM tar)
  if(NOT TAR_PROGRAM)
    message(FATAL_ERROR "Cannot untar data without tar")
  endif()
  execute_process(
    COMMAND ${TAR_PROGRAM} -xf ${NGC3256_TGZ}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/data)
endif()
