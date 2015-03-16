cdef extern from "fftw3.h":
    ctypedef struct fftw_plan:
        pass
    ctypedef struct fftw_complex:
        pass

    cdef fftw_plan fftw_plan_dft_2d( int n0, int n1, fftw_complex *_in,  \
                                     fftw_complex *out,                  \
                                     int sign, unsigned flags )

    cdef void fftw_destroy_plan(fftw_plan plan)
    cdef void fftw_execute(const fftw_plan plan)

    enum:
        FFTW_FORWARD
        FFTW_BACKWARD
        FFTW_MEASURE
        FFTW_ESTIMATE
        FFTW_PATIENT
        FFTW_EXHAUSTIVE

cdef class Fourier2D:
    cdef:
        fftw_plan _plan
        fftw_complex *_data
        object shape
    cpdef execute(self)
    cdef void set_ccall(self, void** data) nogil
