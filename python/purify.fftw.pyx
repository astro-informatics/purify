from cpython.mem cimport PyMem_Malloc, PyMem_Free
from cython.view cimport array as cview

cdef class Fourier2D:
    """ A specialist fft class used in purify.

        Plans of a given size are pre-allocated. This is an in-place operation.
    """
    def __cinit__( self, image_size, oversample, direction = "forward", flags = "measure"):
        """ Allocate memory and plans """
        from numpy import product

        if direction.lower() == "forward": direction = FFTW_FORWARD
        elif direction.lower() == "backward": direction = FFTW_BACKWARD
        else: raise ValueError("direction should be one of 'forward' or 'backward'")

        flagtypes = { 'measure': FFTW_MEASURE, 'estimate': FFTW_ESTIMATE,
                      'patient': FFTW_PATIENT, 'exhaustive': FFTW_EXHAUSTIVE }
        if flags.lower() not in flagtypes.keys():
            raise ValueError('flags should be one of %s' % flagtypes.keys())
        flags = flagtypes[flags.lower()]

        self.shape = oversample[0] * image_size[0], oversample[1] * image_size[1]
        cdef int nsize = product(self.shape) * sizeof(fftw_complex)

        self._data = <fftw_complex*> PyMem_Malloc(nsize)

        self._plan = fftw_plan_dft_2d( self.shape[0], self.shape[1], self._data, self._data,
                                       direction, flags )

    cpdef execute(self):
        """ Executes fft """
        fftw_execute(self._plan)

    def __dealloc__(self):
        """ Deallocates memory and plans """
        fftw_destroy_plan(self._plan)
        PyMem_Free(self._data)

    cdef void set_ccall(self, void** data) nogil:
        """ Sets up void data structures for calls to SOPT and PURIFY functions.

            :Parameters:
                data : void*[2]
                    Will be set to the address of plan and the address of the first element of data.
        """
        data[0] = <void *> &self._plan
        data[1] = <void *> self._data


    property data:
        """ Input and output data of the FFT

            Array is owned by this object. Setting the array changes the value of its elements, not
            the array itself. Eg. setting `fourier.data = x` copies the values of x to the array
            owned by `fourier`.
        """
        def __get__(self):
            from numpy import asarray
            cdef cview data = <double complex [:self.shape[0], :self.shape[1]]> \
                              <double complex*> self._data
            return asarray(data)
        def __set__(self, value): self.data[:] = value
