from cython.view cimport contiguous
from purify.sparse cimport _SparseMatRow, purify_sparsemat_freer, _convert_sparsemat
from purify.visibility cimport _wrap_visibility, _Visibility

cdef extern from "purify_measurement.h":
    ctypedef struct _MeasurementParams "purify_measurement_cparam":
        int nmeas # Number of measurements
        int ny1   # Number of rows in discrete image
        int nx1   # Number of columns in discrete image
        int ofy   # Oversampling factor in row dimension
        int ofx   # Oversampling factor in column dimension
        int ky    # Number of rows in interpolation kernel
        int kx    # Number of columns in interpolation kernel
  

cdef extern from "purify_measurement.h":
    cdef void _kernels "purify_measurement_init_cft"(
            _SparseMatRow *mat, 
            double *deconv, double *u, double *v, 
            _MeasurementParams *param
    )


def kernels(visibility, image_size, oversampling, interpolation_size):
    """ Creates interpolation and deconvolution kernels

        :Parameters:
            visibility: pandas.Dataframe
                Should contain two column, 'u' and 'v'. It needs not be a full visibility dataframe.
            image_size: (int, int)
                Size of the discrete image
            oversampling: (int, int)
                Oversampling factors in both dimensions
            interpolation_size: (int, int)
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

    deconvolution_kernel = zeros(image_size, dtype='double')
    cdef:
        _MeasurementParams params
        _SparseMatRow sparse
        double[:, ::contiguous] c_deconvolution = deconvolution_kernel
        double[::1] u = visibility['u'].values
        double[::1] v = visibility['v'].values

    params.nmeas = len(visibility)
    params.ny1, params.nx1 = image_size
    params.ofy, params.ofx = oversampling
    params.ky, params.kx = interpolation_size

    _kernels(&sparse, &c_deconvolution[0, 0],  &u[0], &v[0], &params)

    interpolation_kernel = _convert_sparsemat(&sparse)
    purify_sparsemat_freer(&sparse)

    Kernel = namedtuple('Kernel', ['interpolation', 'deconvolution'])
    return Kernel(interpolation_kernel, deconvolution_kernel)
