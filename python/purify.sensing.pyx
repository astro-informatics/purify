from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from cython.view cimport contiguous
from purify.sparse cimport purify_sparsemat_freer, _convert_sparsemat, \
                           _wrap_sparsemat
from purify.visibility cimport _wrap_visibility, _Visibility


cdef extern from "purify_measurement.h":
    cdef void purify_measurement_init_cft(
            _SparseMatRow *mat,
            double *deconv, double complex *shifts, double *u, double *v,
            _MeasurementParams *param
    )

cdef void _sensing_params( _MeasurementParams *_params, int _nvis,
                               _image, _oversampling, _interpolation ):
    """ Fills sensing parameter structure. """
    _params.nmeas = _nvis
    _params.ny1, _params.nx1 = _image
    _params.ofy, _params.ofx = _oversampling
    _params.ky, _params.kx = _interpolation

def visibility_column_as_numpy_array(name, visibility):
    """ Makes sure arguments are numpy arrays, or convert them

        This function avoids copies whenever possible.
        The U, V, and Y components should be available from the visibility as
        indices 'u', 'v', 'y', 'w', or 0, 1, 2, 3. The string format is tried
        first.

        If the input column of interest is complex, then a complex numpy aray
        is returned. Otherwise an array of doubles is returned. Complex/real
        arrays with other precision (single, quad...) are converted to double
        precision.
    """
    from numpy import array, iscomplexobj
    # Input name should be explicit, rather than a number
    names = {'u': 0, 'v': 1, 'y': 2, 'w': 3}
    assert name in names.keys()

    try:
        arg = visibility[name]
    except:
        try:
            arg = visibility[names[name]]
        except:
            if name == 3 or name == 'w':
                return None

    # Goes from columns of dataframes to numpy arrays
    arg = getattr(arg, 'values', arg)
    # Convert to expected type, unless None
    if not hasattr(arg, 'dtype'):
        arg = array(arg, dtype='complex' if iscomplexobj(arg) else 'double')
    elif arg.dtype not in ['complex', 'double']:
        arg = arg.astype('complex' if iscomplexobj(arg) else 'double')
    return arg

def kernels(visibility, dimensions, oversampling, interpolation):
    """ Creates interpolation and deconvolution kernels

        :Parameters:
            visibility: pandas.Dataframe
                Should contain two column, 'u' and 'v'. It needs not be a full
                visibility dataframe.  It can also be a dictionary with 'u' and
                'v' items. Finally, it can be a sequence of two arrays with 'u'
                the first one and 'v' the second.
            dimensions: (int, int)
                Size of the discrete image
            oversampling: (int, int)
                Oversampling factors in both dimensions
            interpolation: (int, int)
                Size of the interpolation kernel

        :returns: (interpolation, deconvolution, shifts)
           The interpolation kernel is a sparse CSR matrix, whereas the
           deconvolution kernel is a n by m matrix where n and m are the
           dimensions of the image.

           This is a named tuple object.
    """
    from collections import namedtuple
    from numpy import zeros, product

    py_u = visibility_column_as_numpy_array('u', visibility)
    py_v = visibility_column_as_numpy_array('v', visibility)
    py_shifts = zeros(py_u.shape, dtype="complex128")

    deconvolution_kernel = zeros(dimensions, dtype='double', order="C")
    cdef:
        _MeasurementParams params
        _SparseMatRow sparse
        double* c_deconvolution = <double*>\
            untyped_pointer_to_data(deconvolution_kernel)
        double[::1] c_u = py_u
        double[::1] c_v = py_v
        double complex[::1] c_shifts = py_shifts

    _sensing_params( &params, len(py_u), dimensions, oversampling,
        interpolation )

    purify_measurement_init_cft(
            &sparse, c_deconvolution, &c_shifts[0], &c_u[0], &c_v[0], &params)

    interpolation_kernel = _convert_sparsemat(&sparse).copy()
    purify_sparsemat_freer(&sparse)

    Kernel = namedtuple(
            'Kernel', ['interpolation', 'deconvolution', 'shifts'])
    return Kernel(interpolation_kernel, deconvolution_kernel, py_shifts)

# Forward declaration so we can bind the parent
# argument of _VoidedData.__init__
cdef class SensingOperator

cdef class _VoidedData:
    """ Holds voided data needed by the purify_measurement_cft*. """

    def __init__(self, SensingOperator parent not None, forward, scale=None):
        """ Initializes void structure with info needed by C library """
        self.deconvolution = parent.kernels.deconvolution if scale is None \
                             else parent.kernels.deconvolution * scale
        _wrap_sparsemat(parent.kernels.interpolation, &self._c_sparse)
        self._data[0] = <void *> &parent._params
        self._data[1] = untyped_pointer_to_data(self.deconvolution)
        self._data[2] = <void *> &self._c_sparse

        cdef double complex[::1] c_shifts = parent.kernels.shifts
        self._data[5] = <void*> &c_shifts[0]

        if forward: parent._fftw_forward.set_ccall(&self._data[3])
        else: parent._fftw_backward.set_ccall(&self._data[3])

    cdef void** data(self):
        """ Pointer to void structure of data """
        return &self._data[0]



cdef class SensingOperator:
    """ Forward and adjoint sensing operators for continuous visibilities """
    def __init__( self, visibility, dimensions, oversampling, interpolation,
                  fftwflags = "measure" ):
        """ Creates sensing operator

            :Parameters:
                visibility: pandas.Dataframe
                    Should contain two column, 'u' and 'v'. It needs not be a
                    full visibility dataframe.
                dimensions: (int, int)
                    Size of the discrete image
                oversampling: (int, int)
                    Oversampling factors in both dimensions
                interpolation: (int, int)
                    Size of the interpolation kernel
                fftwflags:
                    Any combination of flags to define the FFTW transforms. See
                    `purify.fftw.Fourier2D`.
        """
        py_u = visibility_column_as_numpy_array('u', visibility)
        py_v = visibility_column_as_numpy_array('v', visibility)

        _sensing_params(&self._params, len(py_u),
                dimensions, oversampling, interpolation)

        self._kernels = kernels((py_u, py_v), dimensions,
                oversampling, interpolation)
        """ Interpolation and deconvolution kernels. """

        self._fftw_forward = Fourier2D(dimensions,
                oversampling, "forward", fftwflags)
        """ Forward fourier transform object """
        self._fftw_backward = Fourier2D(dimensions,
                oversampling, "backward", fftwflags)
        """ Backward fourier transform object """

    property sizes:
        """ Different sizes involved in the problem """
        def __get__(self):
            from collections import namedtuple
            SensingOperatorSizes = namedtuple(
                'SensingOperatorSizes',
                ['measurements', 'image', 'oversampling', 'interpolation']
            )
            return SensingOperatorSizes(
                self._params.nmeas,
                (self._params.ny1, self._params.nx1),
                (self._params.ofy, self._params.ofx),
                (self._params.kx, self._params.ky)
            )

    property kernels:
        """ Interpolation and deconvolution kernels. """
        def __get__(self): return self._kernels

    cpdef forward(self, image):
        """ From image to visibility domain """
        from numpy import zeros
        if image.shape != self.sizes.image:
            raise ValueError("Image is of incorrect size")
        if image.dtype != "complex": image = image.astype("complex")

        visibilities = zeros(self._params.nmeas, dtype="complex", order='C')
        cdef:
            void* c_visibilities = untyped_pointer_to_data(visibilities)
            void* c_image = untyped_pointer_to_data(image)

        data = self.forward_voided_data()

        purify_measurement_cftfwd(c_visibilities, c_image, data.data())

        return visibilities


    cpdef adjoint(self, visibilities):
        """ From visibility to image domain """
        from numpy import zeros
        if len(visibilities) != self._params.nmeas:
            raise ValueError("Visibility is of incorrect size %s vs %s"
                    % (len(visibilities), self._params.nmeas))
        if visibilities.dtype != "complex":
            visibilities = visibilities.astype("complex")

        image = zeros(self.sizes.image, dtype="complex", order='C')
        cdef:
            void* c_image = untyped_pointer_to_data(image)
            void* c_visibilities = untyped_pointer_to_data(visibilities)
        data = self.adjoint_voided_data()

        purify_measurement_cftadj(c_image, c_visibilities, data.data())

        return image

    cdef _VoidedData forward_voided_data(self, scale = None):
        """ Sets C data structures for callbacks. """
        return _VoidedData(self, True, scale)

    cdef _VoidedData adjoint_voided_data(self, scale = None):
        """ Sets C data structures for callbacks. """
        return _VoidedData(self, False, scale)
