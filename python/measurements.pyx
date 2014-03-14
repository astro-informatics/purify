from cython.view cimport contiguous
from purify.sparse cimport _SparseMatRow, purify_sparsemat_freer, _convert_sparsemat, \
                           _wrap_sparsemat
from purify.visibility cimport _wrap_visibility, _Visibility


cdef extern from "purify_measurement.h":
    cdef void purify_measurement_init_cft(
            _SparseMatRow *mat, 
            double *deconv, double *u, double *v, 
            _MeasurementParams *param
    )
    void purify_measurement_cftfwd(void *, void *, void **)
    void purify_measurement_cftadj(void *, void *, void **)

cdef void _measurement_params( _MeasurementParams *_params, int _nmeasurements, _image,
                               _oversampling, _interpolation ):
    """ Fills measurement parameter structure. """
    _params.nmeas = _nmeasurements
    _params.ny1, _params.nx1 = _image
    _params.ofy, _params.ofx = _oversampling
    _params.ky, _params.kx = _interpolation


def kernels(visibility, dimensions, oversampling, interpolation):
    """ Creates interpolation and deconvolution kernels

        :Parameters:
            visibility: pandas.Dataframe
                Should contain two column, 'u' and 'v'. It needs not be a full visibility dataframe.
            dimensions: (int, int)
                Size of the discrete image
            oversampling: (int, int)
                Oversampling factors in both dimensions
            interpolation: (int, int)
                Size of the interpolation kernel

        :returns: (interpolation, deconvolution)
           The interpolation kernel is a sparse CSR matrix, whereas the deconvolution kernel is a n
           by m matrix where n and m are the dimensions of the image.

           This is a named tuple object.
    """
    from collections import namedtuple
    from numpy import zeros, product
    for name in ['u', 'v']:
        if visibility[name].values.dtype != 'double':
            raise TypeError("Visibility's %s column should be composed of doubles." % name)

    deconvolution_kernel = zeros(dimensions, dtype='double')
    cdef:
        _MeasurementParams params
        _SparseMatRow sparse
        double[:, ::contiguous] c_deconvolution = deconvolution_kernel
        double[::1] u = visibility['u'].values
        double[::1] v = visibility['v'].values

    _measurement_params(&params, len(visibility), dimensions, oversampling, interpolation)

    purify_measurement_init_cft(&sparse, &c_deconvolution[0, 0],  &u[0], &v[0], &params)

    interpolation_kernel = _convert_sparsemat(&sparse).copy()
    purify_sparsemat_freer(&sparse)

    Kernel = namedtuple('Kernel', ['interpolation', 'deconvolution'])
    return Kernel(interpolation_kernel, deconvolution_kernel)



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

        self.kernels = kernels(visibility, dimensions, oversampling, interpolation)
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


    cpdef forward(self, image):
        """ Define measurement operator for continuous visibilities """
        from numpy import zeros
        if image.shape != self.sizes.image: raise ValueError("Image is of incorrect size")
        if image.dtype != "complex": image = image.astype("complex")
        
        visibilities = zeros(self._params.nmeas, dtype="complex")
        cdef:
            unsigned char[::1] c_visibilities = visibilities.data
            unsigned char[::1] c_image = image.data
            unsigned char[::1] c_deconv = self.kernels.deconvolution.data
            _SparseMatRow c_sparse
            void* data[5]

        _wrap_sparsemat(self.kernels.interpolation, &c_sparse)
        data[0] = <void *> &self._params
        data[1] = <void *> &c_deconv[0]
        data[2] = <void *> &c_sparse
        self._fftw_forward.set_ccall(&data[3])

        purify_measurement_cftfwd(<void*> &c_visibilities[0], <void*> &c_image[0], &data[0])

        return visibilities

    cpdef adjoint(self, visibilities):
        """ Define adjoint measurement operator for continuous visibilities """
        from numpy import zeros
        if len(visibilities) != self._params.nmeas: raise ValueError("Image is of incorrect size")
        if visibilities.dtype != "complex": visibilities = visibilities.astype("complex")
        
        image = zeros(self.sizes.image, dtype="complex")
        cdef:
            unsigned char[::1] c_image = image.data
            unsigned char[::1] c_visibilities = visibilities.data
            unsigned char[::1] c_deconv = self.kernels.deconvolution.data
            _SparseMatRow c_sparse
            void* data[5]
        _wrap_sparsemat(self.kernels.interpolation, &c_sparse)
        data[0] = <void *> &self._params
        data[1] = <void *> &c_deconv[0]
        data[2] = <void *> &c_sparse
        self._fftw_backward.set_ccall(&data[3])

        purify_measurement_cftadj(<void*> &c_image[0], <void*> &c_visibilities[0], &data[0])

        return image
