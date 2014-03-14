from purify.fftw cimport Fourier2D

# Need this include somehow. Should be there form C-import, but isn't.
cdef extern from "purify_sparsemat.h":
    pass

cdef extern from "purify_measurement.h":
    ctypedef struct _MeasurementParams "purify_measurement_cparam":
        int nmeas # Number of measurements
        int ny1   # Number of rows in discrete image
        int nx1   # Number of columns in discrete image
        int ofy   # Oversampling factor in row dimension
        int ofx   # Oversampling factor in column dimension
        int ky    # Number of rows in interpolation kernel
        int kx    # Number of columns in interpolation kernel
  
cdef class MeasurementOperator:
    cdef:
        _MeasurementParams _params
        Fourier2D _fftw_forward
        Fourier2D _fftw_backward
        object kernels
    cpdef forward(self, image)
    cpdef adjoint(self, visibilities)


