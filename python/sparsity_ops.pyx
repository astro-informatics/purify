from cython.view cimport contiguous
from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef extern from "sopt_sara.h":
    void sopt_sara_initop(sopt_sara_param *param, int nx1, int nx2, int nb_levels, 
                          sopt_wavelet_type *dict_types)
    void sopt_sara_free(sopt_sara_param *param)
    void sopt_sara_analysisop(void *, void *, void **)
    void sopt_sara_synthesisop(void *, void *, void **)

cdef class SparsityOperator:
    """ Mostly opaque object that holds basis-function information """

    wavelet_types = {
        'dirac': SOPT_WAVELET_Dirac,
        'db1':   SOPT_WAVELET_DB1,
        'db2':   SOPT_WAVELET_DB2,
        'db3':   SOPT_WAVELET_DB3,
        'db4':   SOPT_WAVELET_DB4,
        'db5':   SOPT_WAVELET_DB5,
        'db6':   SOPT_WAVELET_DB6,
        'db7':   SOPT_WAVELET_DB7,
        'db8':   SOPT_WAVELET_DB8,
        'db9':   SOPT_WAVELET_DB9,
        'db10':  SOPT_WAVELET_DB10
    }

    def __cinit__(self, shape, unsigned int nlevels, types):
        """ Creates the basis functions.

            :Parameters:
                shape: (int, int)
                    Size of the image
                nlevels: int
                    Number of levels of each wavelet
                types: 
                    Any number of "DB1" through "DB10", or "Dirac"
        """
        types = set([u.lower() for u in types])
        if not (set([u.lower() for u in self.wavelet_types.iterkeys()]) >= types):
            raise ValueError("Wavelet types should one or more of %s" % self.wavelet_types.keys())
        if len(types) == 0: 
            raise ValueError("Cannot create empty basis set")
        
        cdef sopt_wavelet_type* wavelets  \
                = <sopt_wavelet_type*> PyMem_Malloc(len(types) * sizeof(sopt_wavelet_type))

        for i, wavelet in enumerate(types): wavelets[i] = self.wavelet_types[wavelet.lower()]

        self._wavelets.ndict = len(types)
        self._wavelets.real = 0;
        sopt_sara_initop(&self._wavelets, shape[0], shape[1], <int>nlevels, wavelets)
        
        PyMem_Free(wavelets)

    def __dealloc__(self):
        sopt_sara_free(&self._wavelets)
        self._wavelets.wav_params = NULL
        self._wavelets.ndict = 0

    def __len__(self):
        """ Number of wavelet basis """
        return self._wavelets.ndict

    def __getitem__(self, index):
        """ Returns tuple with information from wavelets """
        from numpy import asarray
        from collections import namedtuple
        if index < 0: index += len(self)
        if index < 0 or index >= len(self): raise IndexError("SparsityOperator")
        
        Wavelet = namedtuple('Wavelet', ['name', 'type', 'shape', 'nlevels', 'filter'])

        cdef sopt_wavelet_param* wavelet = &self._wavelets.wav_params[index]
        for name, type in self.wavelet_types.iteritems():
            if type == wavelet.type: break
        else: raise RuntimeError("Could not determine basis function type")

        filter = asarray(<double[:wavelet.h_size:1]> wavelet.h)
        filter.flags.writeable = False
        return Wavelet( name=name, type=wavelet.type, shape=(wavelet.nx1, wavelet.nx2),
                        nlevels=wavelet.nb_levels, filter=filter )

    def __iter__(self):
        """ Iterates over wavelets """
        def generator():
            for i in xrange(len(self)): yield self[i]
        return generator()

    cpdef analyze(self, image):
        """ computes the analysis operator for concatenation of bases 

            :Parameters:
                image: numpy.ndarray
                    Should have the same shape as the wavelets in this object.
                    Should be real if the wavelets are real, and real or complex if the wavelets are
                    complex.
        """
        from numpy import zeros, dtype as np_dtype

        # Checks size of input
        assert len(set([u.shape for u in self])) == 1
        if self[0].shape != image.shape: raise ValueError("Incorrect image shape.")

        # Checks type of the input and transform it if need be 
        dtype = np_dtype('double' if self._wavelets.real else 'complex')
        if dtype != image.dtype: image = image.astype(dtype)

        # Creates ouput and calls C function
        result = zeros((len(self),) + image.shape, dtype=dtype)
        cdef:
            unsigned char[::1] c_image = image.data
            unsigned char[::1] c_result = result.data
            void *c_voidify = <void*> &self._wavelets
        sopt_sara_analysisop(<void*> &c_result[0], <void*> &c_image[0], &c_voidify)

        return result

    cpdef synthesize(self, inout):
        """ This function computes the synthesis operator for concatenation of bases.
            
            For parameters, see sopt_sara_synthesisop.
        """
        from numpy import zeros, dtype as np_dtype

        # Checks size of input
        assert len(set([u.shape for u in self])) == 1
        if ((len(self),) + self[0].shape) != inout.shape:
            raise ValueError("Incorrect input shape.")

        # Checks type of the input and transform it if need be 
        dtype = np_dtype('double' if self._wavelets.real else 'complex')
        if dtype != inout.dtype: inout = inout.astype(dtype)

        # Creates ouput and calls C function
        result = zeros(self[0].shape, dtype=dtype)
        cdef:
            unsigned char[::1] c_inout = inout.data
            unsigned char[::1] c_result = result.data
            void *c_voidify = <void*> &self._wavelets
        sopt_sara_synthesisop(<void*> &c_result[0], <void*> &c_inout[0], &c_voidify)

        return result
