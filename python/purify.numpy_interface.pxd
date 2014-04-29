# Use numpy interface rather than generic buffer interface
# for the sake of older numpys without the newer python buffer interface.
# The main issue is accessing pointers to the underlying data for
# both complex and real arrays.
# This is what the buffer interface allows us to do.
# On the other hand, the normal cython numpy interface expects an explicit
# type. It is simpler to roll out are own typeless for those arrays that
# could be either complex or real.
# In a few years / CASA upgrades, it may be forthwhile to replace all calls to
# this interface with the normal cython buffer interface. One should look at the
# changeset when this particular file was introduced.
cdef extern from "numpy/arrayobject.h":
    ctypedef struct PyArrayObject:
        pass
    void* PyArray_DATA(PyArrayObject*)
    void import_array()

cdef inline void* untyped_pointer_to_data(object array) except *:
    from numpy import ndarray
    if not isinstance(array, ndarray):
        raise TypeError("Expected array numpy array on input")
    if not (array.flags['C_CONTIGUOUS'] or array.flags['F_CONTIGUOUS']):
        msg = "Expected a C or Fortan contiguous numpy array on input"
        raise TypeError(msg)
    cdef void* data = PyArray_DATA(<PyArrayObject*>array)
    return data
