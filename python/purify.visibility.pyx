cdef extern from "purify_visibility.h":

    ctypedef enum _VISIBILITY_FILETYPES "purify_visibility_filetype":
        VIS "PURIFY_VISIBILITY_FILETYPE_VIS"
        PROFILE_VIS "PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS"
        PROFILE_WIS "PURIFY_VISIBILITY_FILETYPE_PROFILE_WIS"

    cdef int purify_visibility_readfile(
            _Visibility *visibility,
            const char *filename,
            _VISIBILITY_FILETYPES filetype
    )

    cdef void purify_visibility_free(_Visibility *_vis)

cdef object _convert_visibility(_Visibility *_visibility):
    """ Copies a C visibility iNto a dataframe """
    from pandas import DataFrame
    from numpy import array

    cdef int N = _visibility[0].nmeas
    cdef:
      double[::1] u = <double[:N:]> _visibility[0].u
      double[::1] v = <double[:N:]> _visibility[0].v
      double[::1] w = <double[:N:]> _visibility[0].w
      double complex[::1] noise = <double complex[:N:]> _visibility[0].noise_std
      double complex[::1] y = <double complex[:N:]> _visibility[0].y

    return DataFrame({
      'u': array(u), 'v': array(v), 'w': array(w), 'noise': array(noise), 'y': array(y)
    })

cdef void _wrap_visibility(py_visibility, _Visibility *c_visibility) except *:
    """ Wraps a visibility c-structure around python dataframe """
    from itertools import repeat
    for name, type in list(zip(['u', 'v', 'w'], repeat('double'))) \
                      + list(zip(['noise', 'y'], repeat('complex'))):
        if not py_visibility[name].values.dtype == type:
            message = "Visibility's %s column should be composed of %s values." % (name, type)
            raise TypeError(message)
    cdef:
        double[::1] u = py_visibility['u'].values
        double[::1] v = py_visibility['v'].values
        double[::1] w = py_visibility['w'].values
        double complex[::1] noise = py_visibility['noise'].values
        double complex[::1] y = py_visibility['y'].values

    c_visibility.nmeas = len(py_visibility)
    c_visibility.u = &u[0]
    c_visibility.v = &v[0]
    c_visibility.w = &w[0]
    c_visibility.noise_std = &noise[0]
    c_visibility.y = &y[0]


def read_visibility(const char * filename):
    """ Reads visibility from input file

        :Parameters:
          filename: Name of the file storing the visibility
    """
    from os.path import exists
    if not exists(filename): raise IOError("File %s does not exist" % filename)

    cdef:
        _Visibility visibility
        _VISIBILITY_FILETYPES flag = PROFILE_VIS

    result = purify_visibility_readfile(&visibility, filename, flag)
    data = None if result < 0 else _convert_visibility(&visibility)
    purify_visibility_free(&visibility)
    if result == -1:
      raise IOError("Could not open file %s" % filename)
    elif result == -2:
      raise IOError("Unexpected end of file %s" % filename)
    elif result < 0:
      raise Exception("Unknown error when opening %s" % filename)

    return data
