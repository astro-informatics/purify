if(NOT CasaCore_INCLUDE_DIR)
  find_path(CasaCore_INCLUDE_DIR NAMES casacore/casa/version.h
    PATHS ${CASACORE_PREFIX} ENV CASACORE_PREFIX CPATHS)
endif()

# Grep version from file
if(EXISTS "${CasaCore_INCLUDE_DIR}/casacore/casa/version.h")
  file(READ "${CasaCore_INCLUDE_DIR}/casacore/casa/version.h" version_txt)
  if("${version_txt}" MATCHES "CASACORE_MAJOR_VERSION ([0-9]+)")
    set(CasaCore_VERSION "${CMAKE_MATCH_0}")
  endif()
  if("${version_txt}" MATCHES "CASACORE_MINOR_VERSION ([0-9]+)")
    set(CasaCore_VERSION "${CasaCore_VERSION}.${CMAKE_MATCH_0}")
  endif()
  if("${version_txt}" MATCHES "CASACORE_PATCH_VERSION ([0-9]+)")
    set(CasaCore_VERSION "${CasaCore_VERSION}.${CMAKE_MATCH_0}")
  endif()
endif()

# Define the Casacore components' inter-dependencies.
set(CasaCore_components_DEPENDENCIES  coordinates)
set(CasaCore_coordinates_DEPENDENCIES fits)
set(CasaCore_fits_DEPENDENCIES        measures)
set(CasaCore_images_DEPENDENCIES      components lattices mirlib)
set(CasaCore_lattices_DEPENDENCIES    tables scimath)
set(CasaCore_measures_DEPENDENCIES    tables scimath)
set(CasaCore_ms_DEPENDENCIES          measures)
set(CasaCore_msfits_DEPENDENCIES      fits ms)
set(CasaCore_scimath_DEPENDENCIES     scimath_f)
set(CasaCore_scimath_f_DEPENDENCIES   casa)
set(CasaCore_tables_DEPENDENCIES      casa)

# Creates a list with deepest dependency first
# No cmake does not allow recursive function calls
function(casacore_depth_first component output)
  set(index 0)
  set(all_components ${component})
  list(LENGTH all_components last)
  while(last GREATER index)
    list(GET all_components ${index} item)
    foreach(subdep ${CasaCore_${item}_DEPENDENCIES})
      list(FIND all_components ${subdep} found)
      if(found EQUAL -1)
        list(APPEND all_components ${subdep})
      endif()
    endforeach()

    list(LENGTH all_components last)
    math(EXPR index "${index} + 1")
  endwhile()
  list(REVERSE all_components)
  set(${output} ${all_components} PARENT_SCOPE)
endfunction()

# Check component and its direct and indirect dependencies exist
# Does not create targets
function(casacore_check_component_dependencies component output)
  casacore_depth_first(${component} all_components)
  set(found TRUE)
  foreach(dependency ${all_components})
    find_library(CasaCore_${dependency}_LIBRARY
            NAMES casa_${dependency} PATHS ENV CASACORE_PREFIX)
    if(NOT CasaCore_${dependency}_LIBRARY)
      set(found FALSE)
      break()
    endif()
  endforeach()
  set(${output} ${found} PARENT_SCOPE)
endfunction()

# Creates an imported library for the component and each of its direct and indirect dependencies
function(casacore_create_imported_libs component)
  casacore_depth_first(${component} all_components)
  foreach(subcomponent ${all_components})
    if(TARGET casacore::${subcomponent})
      continue()
    endif()
    unset(dependencies)
    foreach(subdep ${CasaCore_${subcomponent}_DEPENDENCIES})
      list(APPEND dependencies casacore::${subdep})
      if(NOT TARGET casacore::${subdep})
        message(FATAL_ERROR "internal bug")
      endif()
    endforeach()

    get_filename_component(extension "${CasaCore_${subcomponent}_LIBRARY}" EXT)
    if(extension STREQUAL "${CMAKE_SHARED_LIBRARY_SUFFIX}")
      add_library(casacore::${subcomponent} SHARED IMPORTED)
    elseif(extension STREQUAL "${CMAKE_STATIC_LIBRARY_SUFFIX}")
      add_library(casacore::${subcomponent} STATIC IMPORTED)
    else()
      add_library(casacore::${subcomponent} UNKNOWN IMPORTED)
    endif()
    set_target_properties(casacore::${subcomponent} PROPERTIES
      IMPORTED_LOCATION "${CasaCore_${subcomponent}_LIBRARY}"
      INTERFACE_LINK_LIBRARIES "${dependencies}"
      INTERFACE_INCLUDE_DIRECTORIES "${CasaCore_INCLUDE_DIR}"
    )
    set(CasaCore_COMPONENT_${subcomponent}_FOUND TRUE)
    list(APPEND CasaCore_LIBRARIES casacore::${subcomponent})
  endforeach()
  set(CasaCore_LIBRARIES ${CasaCore_LIBRARIES} PARENT_SCOPE)

endfunction()

# Brings together the checks for existence and the imported library creation
function(casacore_add_component_library component)
  casacore_check_component_dependencies(${component} allfound)

  if(allfound)
    casacore_create_imported_libs(${component})
    set(CasaCore_LIBRARIES ${CasaCore_LIBRARIES} PARENT_SCOPE)
  elseif(NOT allfound AND CasaCore_FIND_REQUIRED_${component})
    message(FATAL_ERROR "Dependencies for casa component ${component} could not all be found")
  endif()
endfunction()

# adds imported libraries for each requested component and for the core itself
unset(CasaCore_LIBRARIES)
foreach(module casa ${CasaCore_FIND_COMPONENTS})
  casacore_add_component_library(${module})
endforeach()
if(CasaCore_LIBRARIES)
  list(REMOVE_DUPLICATES CasaCore_LIBRARIES)
endif()

# handle the QUIETLY and REQUIRED arguments and set CASACORE_FOUND to TRUE if.
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CasaCore DEFAULT_MSG CasaCore_LIBRARIES
  CasaCore_INCLUDE_DIR CasaCore_VERSION)
