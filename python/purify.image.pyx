import cython
cdef extern from "purify_image.h":
    ctypedef struct _Image "purify_image":
        double fov_x
        double fov_y
        int nx
        int ny
        double *pix

    ctypedef enum _IMAGE_FILETYPES "purify_image_filetype":
        FITS_DOUBLE "PURIFY_IMAGE_FILETYPE_FITS"
        FITS_FLOAT "PURIFY_IMAGE_FILETYPE_FITS_FLOAT"

    cdef void purify_image_free(_Image *img)
    cdef int purify_image_readfile( _Image *img, const char *filename,
                                    _IMAGE_FILETYPES filetype )

    cdef int purify_image_writefile(_Image *img, const char *filename, _IMAGE_FILETYPES filetype)


cdef class Image:
    """ Purify image object. """
    cdef:
        double _fov[2]
        object _pixels

    def __init__(self, filename = None, fov=None):
        """ Initializes the image

            :Parameters:
                filename : File from which to read the image data
                fov: (float, float)
                    Field of view. Not used anywhere...
        """
        from numpy import array
        self._pixels = array([[]], dtype="double")
        """ Wrapper around image data """
        self.fov = (0, 0) if fov is None else fov
        if filename is not None: self.read(filename)

    def read(self, const char * filename):
        """ Reads image from file """
        from numpy import asarray
        from os.path import exists
        if not exists(filename): raise IOError("File %s does not exist" % filename)

        cdef:
            _Image image
            _IMAGE_FILETYPES flag = FITS_DOUBLE
        read_result = purify_image_readfile(&image, filename, flag)
        if read_result != 0:
            raise RuntimeError("Unknown error when opening %s" % filename)
        cdef double[:, ::1] values = <double [:image.nx, :image.ny:1]> image.pix
        self.pixels = values
        purify_image_free(&image)


    def write(self, const char * filename, asfloat = True):
        """ Writes image to file """
        cdef:
            _Image image
            _IMAGE_FILETYPES flag = FITS_FLOAT if asfloat else FITS_DOUBLE
        self._fill_cpointer(&image)
        read_result = purify_image_writefile(&image, filename, flag)
        if read_result != 0:
            raise RuntimeError("Unknown error when writing to %s" % filename)

    cdef void _fill_cpointer(self, _Image *_image):
        """ Fills a pointer to an image with info from this instance. """
        _image.fov_x = self._fov[0]
        _image.fov_y = self._fov[1]
        _image.nx    = self._pixels.shape[0]
        _image.ny    = self._pixels.shape[1]
        cdef double[:, ::cython.view.contiguous] pixels = self._pixels
        _image.pix   = &pixels[0, 0]

    property shape:
        """ Shape of the image. """
        def __get__(self): return (0, 0) if self._pixels is None else self._pixels.shape
    property ndim:
        """ Number of dimension. """
        def __get__(self): return 0 if self._pixels is None else self._pixels.ndim
    property dtype:
        """ Number of dimension. """
        def __get__(self): return None if self._pixels is None else self._pixels.dtype
    property pixels:
        """ A numpy array holding the pixels of the image """
        def __get__(self): return self._pixels
        def __set__(self, value):
            from numpy import array
            self._pixels = array(value, dtype="float64")
    property fov:
        """ Field of view """
        def __get__(self): return self._fov[0], self._fov[1]
        def __set__(self, value): self._fov[0], self._fov[1] = value[0], value[1]
