# Sets of python library to absolute path
# CASA 4.2.2 does stuff to the environment variables such that the python
# library cannot be found at runtime. So we make it an absolute dependency by
# hand.
function(change_object_rpath object)
        execute_process(COMMAND
            install_name_tool -add_rpath ${directory} ${object}
            ERROR_QUIET
        )
    endforeach()
endfunction()

get_filename_component(libfilename "@PYTHON_LIBRARIES@" NAME)
file(GLOB modules "@PYTHON_PKG_DIR@/purify/*.so")
file(GLOB testmodules "@PYTHON_PKG_DIR@/purify/tests/*.so")
# Loop over each shared object and add rpaths and change tbbs
foreach(library ${modules} ${testmodules})
    execute_process(COMMAND
        install_name_tool -change ${libfilename} @PYTHON_LIBRARIES@ ${library}
        ERROR_QUIET
    )
endforeach()

