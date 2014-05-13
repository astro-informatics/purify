from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from cython.view cimport contiguous
from purify.sparse cimport purify_sparsemat_freer, _convert_sparsemat, \
                           _wrap_sparsemat
from purify.visibility cimport _wrap_visibility, _Visibility


cdef extern from "purify_measurement.h":
    cdef void purify_measurement_init_cft(
            _SparseMatRow *mat,
            double *deconv, double *u, double *v,
            _MeasurementParams *param
    )

cdef void _measurement_params( _MeasurementParams *_params, int _nmeasurements,
                               _image, _oversampling, _interpolation ):
    """ Fills measurement parameter structure. """
    _params.nmeas = _nmeasurements
    _params.ny1, _params.nx1 = _image
    _params.ofy, _params.ofx = _oversampling
    _params.ky, _params.kx = _interpolation


def kernels(visibility, dimensions, oversampling, interpolation):
    """ Creates interpolation and deconvolution kernels

        :Parameters:
            visibility: pandas.Dataframe
                Should contain two column, 'u' and 'v'. It needs not be a full
                visibility dataframe.
            dimensions: (int, int)
                Size of the discrete image
            oversampling: (int, int)
                Oversampling factors in both dimensions
            interpolation: (int, int)
                Size of the interpolation kernel

        :returns: (interpolation, deconvolution)
           The interpolation kernel is a sparse CSR matrix, whereas the
           deconvolution kernel is a n by m matrix where n and m are the
           dimensions of the image.

           This is a named tuple object.
    """
    from collections import namedtuple
    from numpy import zeros, product
    for name in ['u', 'v']:
        if visibility[name].values.dtype != 'double':
            msg = "Visibility['%s'] should be composed of doubles." % name
            raise TypeError(msg)

    deconvolution_kernel = zeros(dimensions, dtype='double', order="C")
    cdef:
        _MeasurementParams params
        _SparseMatRow sparse
        double* c_deconvolution = <double*>\
            untyped_pointer_to_data(deconvolution_kernel)
        double[::1] u = visibility['u'].values
        double[::1] v = visibility['v'].values

    _measurement_params( &params, len(visibility), dimensions, oversampling,
        interpolation )

    purify_measurement_init_cft(&sparse, c_deconvolution,  &u[0], &v[0], &params)

    interpolation_kernel = _convert_sparsemat(&sparse).copy()
    purify_sparsemat_freer(&sparse)

    Kernel = namedtuple('Kernel', ['interpolation', 'deconvolution'])
    return Kernel(interpolation_kernel, deconvolution_kernel)

# Forward declaration so we can bind the parent argument of _VoidedData.__init__
cdef class MeasurementOperator

cdef class _VoidedData:
    """ Holds voided data needed by the purify_measurement_cft*. """

    def __init__(self, MeasurementOperator parent not None, is_forward, scale=None):
        """ Initializes void structure with info needed by C library """
        self.deconvolution = parent.kernels.deconvolution if scale is None \
                             else parent.kernels.deconvolution * scale
        _wrap_sparsemat(parent.kernels.interpolation, &self._c_sparse)
        self._data[0] = <void *> &parent._params
        self._data[1] = untyped_pointer_to_data(self.deconvolution)
        self._data[2] = <void *> &self._c_sparse

        if is_forward: parent._fftw_forward.set_ccall(&self._data[3])
        else: parent._fftw_backward.set_ccall(&self._data[3])

    cdef void** data(self):
        """ Pointer to void structure of data """
        return &self._data[0]



cdef class MeasurementOperator:
    """ Forward and adjoint measurement operators for continuous visibilities """
    def __init__( self, visibility, dimensions, oversampling, interpolation,
                  fftwflags = "measure" ):
        """ Creates measurements.

            :Parameters:
                Should contain two column, 'u' and 'v'. It needs not be a full visibility dataframe.
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
        _measurement_params(&self._params, len(visibility), dimensions, oversampling, interpolation)

        self._kernels = kernels(visibility, dimensions, oversampling, interpolation)
        """ Interpolation and deconvolution kernels. """

        self._fftw_forward = Fourier2D(dimensions, oversampling, "forward", fftwflags)
        """ Forward fourier transform object """
        self._fftw_backward = Fourier2D(dimensions, oversampling, "backward", fftwflags)
        """ Backward fourier transform object """

    property sizes:
        """ Different sizes involved in the problem """
        def __get__(self):
            from collections import namedtuple
            MeasurementSizes = namedtuple(
                'MeasurementSizes',
                ['measurements', 'image', 'oversampling', 'interpolation']
            )
            return MeasurementSizes(
                self._params.nmeas,
                (self._params.ny1, self._params.nx1),
                (self._params.ofy, self._params.ofx),
                (self._params.kx, self._params.ky)
            )

    property kernels:
        """ Interpolation and deconvolution kernels. """
        def __get__(self): return self._kernels

    cpdef forward(self, image):
        """ Define measurement operator for continuous visibilities """
        from numpy import zeros
        if image.shape != self.sizes.image: raise ValueError("Image is of incorrect size")
        if image.dtype != "complex": image = image.astype("complex")

        visibilities = zeros(self._params.nmeas, dtype="complex", order='C')
        cdef:
            void* c_visibilities = untyped_pointer_to_data(visibilities)
            void* c_image = untyped_pointer_to_data(image)

        data = self.forward_voided_data()

        purify_measurement_cftfwd(c_visibilities, c_image, data.data())

        return visibilities


    cpdef adjoint(self, visibilities):
        """ Define adjoint measurement operator for continuous visibilities """
        from numpy import zeros
        if len(visibilities) != self._params.nmeas:
            raise ValueError("Visibility is of incorrect size")
        if visibilities.dtype != "complex": visibilities = visibilities.astype("complex")

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
