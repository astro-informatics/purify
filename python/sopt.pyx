from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef extern from "sopt_sara.h":
    void sopt_sara_initop(sopt_sara_param *param, int nx1, int nx2, int nb_levels, 
                          sopt_wavelet_type *dict_types)
    void sopt_sara_free(sopt_sara_param *param)

cdef class BasisFunctions:
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

        self.wavelets.ndict = len(types)
        self.wavelets.real = 0;
        sopt_sara_initop(&self.wavelets, shape[0], shape[1], <int>nlevels, wavelets)
        
        PyMem_Free(wavelets)

    def __dealloc__(self):
        sopt_sara_free(&self.wavelets)

    def __len__(self):
        """ Number of wavelet basis """
        return self.wavelets.ndict

    def __getitem__(self, index):
        """ Returns names of wavelets """
        if index < 0: index += len(self)
        if index < 0 or index >= len(self): raise IndexError("BasisFunctions")
        value = self.wavelets.wav_params[index].type
        for name, type in self.wavelet_types.iteritems():
            if type == value: return name
        raise RuntimeError("Could not determine basis function type")

    def __iter__(self):
        """ Iterates over wavelets """
        def generator():
            for i in xrange(len(self)): yield self[i]
        return generator()

    property nlevels:
        """ Number of levels for each wavelets """
        def __get__(self):
            return self.wavelets.wav_params.nb_levels

    property shape:
        """ Planar dimensions """
        def __get__(self):
            return self.wavelets.wav_params.nx1, self.wavelets.wav_params.nx2
