from purify.visibility cimport _convert_visibility, _wrap_visibility, _Visibility

def _bindings_cycle(visibility):
    """ Wraps input to C, and converts it back to python """
    cdef _Visibility c_visibility
    _wrap_visibility(visibility, &c_visibility)
    print "AM HERE"
    return _convert_visibility(&c_visibility)
