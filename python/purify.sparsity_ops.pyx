from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from cython.view cimport contiguous
from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef extern from "sopt_sara.h":
    void sopt_sara_initop(sopt_sara_param *param, int nx1, int nx2, int nb_levels,
                          sopt_wavelet_type *dict_types)
    void sopt_sara_free(sopt_sara_param *param)


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

    def __cinit__(self, image_size=(256, 256), unsigned int nlevels=4,
                  wavelets='DB1', **kwargs):
        """ Creates the basis functions.

            :Parameters:
                image_size: (int, int)
                    Size of the image
                nlevels: int
                    Number of levels of each wavelet
                wavelets:
                    Any number of "DB1" through "DB10", or "Dirac"
        """
        allwavelets = set([u.lower() for u in self.wavelet_types.iterkeys()])
        if isinstance(wavelets, str):
            if wavelets.lower() == "all": wavelets = allwavelets;
            else: wavelets = set([wavelets.lower()])
        else: wavelets = set([u.lower() for u in wavelets])
        if not (allwavelets  >= wavelets):
            raise ValueError("Acceptable wavelets: %s" % allwavelets)
        if len(wavelets) == 0:
            raise ValueError("Cannot create empty basis set")

        # Reorders wavelets to follow input scheme from C examples.
        # Makes it easier to compare quantities between two code bases.
        allwavelets = ["db%i" % i for i in range(1, 11)] + ['dirac']
        # makes sure line above contains all wavelets, e.g. it is
        # in sync with self.wavelet_types
        assert wavelets <= set(allwavelets)
        wavelets = [u for u in allwavelets if u in wavelets]

        # Creates C list of wavelets
        cdef int dictsize = len(wavelets) * sizeof(sopt_wavelet_type)
        cdef sopt_wavelet_type* dictionary  \
                = <sopt_wavelet_type*> PyMem_Malloc(dictsize)
        try:
          for cindex, wavelet in enumerate(wavelets):
              dictionary[cindex] = self.wavelet_types[wavelet]

          self._wavelets.ndict = len(wavelets)
          self._wavelets.real = 0;
          sopt_sara_initop( &self._wavelets, image_size[0], image_size[1],
                            <int>nlevels, dictionary )

        finally:
            PyMem_Free(dictionary)

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

        Wavelet = namedtuple('Wavelet', ['name', 'type', 'image_size', 'nlevels', 'filter'])

        cdef sopt_wavelet_param* wavelet = &self._wavelets.wav_params[index]
        for name, type in self.wavelet_types.iteritems():
            if type == wavelet.type: break
        else: raise RuntimeError("Could not determine basis function type")

        filter = asarray(<double[:wavelet.h_size:1]> wavelet.h)
        filter.flags.writeable = False
        return Wavelet( name=name, type=wavelet.type, image_size=(wavelet.nx1, wavelet.nx2),
                        nlevels=wavelet.nb_levels, filter=filter )

    def __iter__(self):
        """ Iterates over wavelets """
        def generator():
            for i in xrange(len(self)): yield self[i]
        return generator()

    cdef set_wavelet_pointer(self, void **data):
        """ Sets pointer for C callbacks. """
        data[0] = <void*>&self._wavelets

    cpdef analyze(self, image):
        """ computes the analysis operator for concatenation of bases

            :Parameters:
                image: numpy.ndarray
                    Should have the same shape as the wavelets in this object.
                    Should be real if the wavelets are real, and real or
                    complex if the wavelets are complex.
        """
        from numpy import zeros, dtype as np_dtype

        # Checks size of input
        assert len(set([u.image_size for u in self])) == 1
        if self[0].image_size != image.shape:
            raise ValueError("Incorrect image size.")

        # Checks type of the input and transform it if need be
        dtype = np_dtype('double' if self._wavelets.real else 'complex')
        if dtype != image.dtype: image = image.astype(dtype)
        if not (image.flags['C_CONTIGUOUS'] or image.flags['F_CONTIGUOUS']):
            msg = "Expected a C or Fortan contiguous numpy array on input"
            raise TypeError("msg")

        # Creates ouput and calls C function
        result = zeros((len(self),) + image.shape, dtype=dtype, order="C")
        if not (result.flags['C_CONTIGUOUS'] or result.flags['F_CONTIGUOUS']):
            msg = "Expected a C or Fortan contiguous numpy array"
            raise RuntimeError("msg")
        cdef:
            void* c_image = untyped_pointer_to_data(image)
            void* c_result = untyped_pointer_to_data(result)
            void* c_voidify
        self.set_wavelet_pointer(&c_voidify)
        sopt_sara_analysisop(c_result, c_image, &c_voidify)

        return result

    cpdef synthesize(self, inout):
        """ Computes the synthesis operator for concatenation of bases. """
        from numpy import zeros, dtype as np_dtype

        # Checks size of input
        assert len(set([u.image_size for u in self])) == 1
        if ((len(self),) + self[0].image_size) != inout.shape:
            raise ValueError("Incorrect input shape.")

        # Checks type of the input and transform it if need be
        dtype = np_dtype('double' if self._wavelets.real else 'complex')
        if dtype != inout.dtype: inout = inout.astype(dtype)
        if not (inout.flags['C_CONTIGUOUS'] or inout.flags['F_CONTIGUOUS']):
            msg = "Expected a C or Fortan contiguous numpy array on input"
            raise TypeError("msg")

        # Creates ouput and calls C function
        result = zeros(self[0].image_size, dtype=dtype, order="C")
        if not (result.flags['C_CONTIGUOUS'] or result.flags['F_CONTIGUOUS']):
            msg = "Expected a C or Fortan contiguous numpy array"
            raise RuntimeError("msg")
        cdef:
            void* c_inout = untyped_pointer_to_data(inout)
            void* c_result = untyped_pointer_to_data(result)
            void *c_voidify
        self.set_wavelet_pointer(&c_voidify)
        sopt_sara_synthesisop(c_result, c_inout, &c_voidify)

        return result
