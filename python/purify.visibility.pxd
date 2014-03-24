# Visibility structure that will be needed for other bindings.
cdef extern from "purify_visibility.h":
    ctypedef struct _Visibility "purify_visibility":
        int nmeas
        double* u
        double* v
        double* w
        double complex *noise_std
        double complex *y

cdef object _convert_visibility(_Visibility *_visibility)
cdef void _wrap_visibility(py_visibility, _Visibility *c_visibility) except *
