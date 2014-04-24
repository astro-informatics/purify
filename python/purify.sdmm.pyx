from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from . import params
from purify.numpy_interface cimport untyped_pointer_to_data
from purify.measurements cimport purify_measurement_cftfwd, \
                                 purify_measurement_cftadj, \
                                 MeasurementOperator, _VoidedData
from purify.sparsity_ops cimport sopt_sara_analysisop, sopt_sara_synthesisop, \
                                 SparsityOperator

cdef extern from "sopt_l1.h":
    ctypedef struct sopt_l1_sdmmparam:
        int verbose
        int max_iter
        double gamma
        double rel_obj
        double epsilon
        double epsilon_tol
        int real_data
        int cg_max_iter
        double cg_tol

    cdef void sopt_l1_sdmm( void *xsol, int nx,
            void (*A)(void *out, void *invec, void **data),
            void **A_data, void (*At)(void *out, void *invec, void **data),
            void **At_data, void (*Psi)(void *out, void *invec, void **data),
            void **Psi_data, void (*Psit)(void *out, void *invec, void **data),
            void **Psit_data, int nr, void *y, int ny, double *weights,
            sopt_l1_sdmmparam param ) nogil

cdef void _convert_sdmm_params(
        sopt_l1_sdmmparam *c_params,
        sdmm, MeasurementOperator measurements, visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[sdmm.verbose]
    c_params.max_iter = sdmm.max_iter
    nelements = product(measurements.sizes.image)
    if sdmm.gamma is not None: c_params.gamma = sdmm.gamma
    else:
        scale = sqrt(nelements) / sqrt(len(visibility))
        xout = measurements.adjoint(visibility['y'])
        c_params.gamma = 1e-3 * max(sdmm.analyze(xout).real) * scale
    c_params.rel_obj = sdmm.relative_variation
    if sdmm.radius is not None: c_params.epsilon = sdmm.radius
    else:
        nvis = float(len(visibility['y']))
        sigma = norm(visibility['y']) * 10.0**-(1.5) / sqrt(nvis)
        c_params.epsilon = sqrt(nvis + 2.0 * sqrt(nvis)) * sigma \
                           / nvis * nelements

    c_params.epsilon_tol = sdmm.relative_radius
    c_params.cg_max_iter = sdmm.cg.max_iter
    c_params.cg_tol = sdmm.cg.tolerance
    if visibility['y'].values.dtype != 'complex':
        raise NotImplemented('Cannot purify real visibilities')
    c_params.real_data = 0

class SDMM(params.Measurements, params.SDMM, SparsityOperator):
    """ Performs Simultaneous Direction Method of Mulitpliers

        Once instantiated for a give output image size and a given input set
        wavelet basis, this function can be used to recover the image for any
        number of visibility data sets:

            >>> from purify import SDMM, read_visibility
            >>> sdmm = SDMM(...) # see below
            >>> visibility = read_visibility("file.vis")
            >>> image = sdmm(visibility)

        There are a fair number of parameters, from the size of the output
        image, to the type of wavelet basis sets that should be used to
        reconstruct it. Please see SDMM.__init__ for details. After
        instanciation, most of these parameters can be accessed and modified as
        attributes of the function object.
    """
    def __init__(self, **kwargs):
        """ Creates a SDMM function object

            :Parameters:
                image_size: (int, int)
                    Size of the reconstructed image. Defaults to `(256, 256)`.
                nlevels: int
                    Number of levels for each wavelet basis set. Defaults to 4.
                wavelets: str or [str]
                    Any number of strings specifying the wavelet basis types.
                    Should be one of "DBn" where $n = 0, 10$, or "Dirac".
                    Defaults to "DB1"
                interpolation: (int, int)
                    Size of the interpolation kernel. Defaults to (24, 24).
                oversampling: (int, int)
                    Default to (2, 2)
                gamma: None or float
                    A convergence criteria. Computed from the input visibility
                    if None. Defaults to None.
                radius: None or float
                    Radius of the L2 Ball. Computed from the input visibility if
                    None. Defaults to None.
                relative_radius: float
                    Defines the range of the radius. Default 1e-2.
                max_iter: int
                    Maximum number of SDMM iterations. Defaults to 300.
                relative_variation: float
                    Convergence criteria against changes in the objective
                    function. Defaults to 1e-4.
                verbose: off|medium|high
                    String indicating the level of verbosity. Defaults to
                    'high'.
                cg_max_iter: int
                    Maximum number of iterations of the internal conjugate
                    gradient operations. Defaults to 100.
                cg_tolerance: float
                    Convergence criteria for the internal conjugate gradient
                    operations. Defaults to 1e-6
        """
        super(SDMM, self).__init__(**kwargs)

    @params.apply_params
    def __call__(self, visibility, image=None, weights=None):
        """ Computes image from input visibility.

            :Parameters:
                visibility:
                    A pandas dataframe with a 'u', 'v', 'w',
                    'y' (visibility per se) columns
                weights:
                    If present, an array of weights of shape
                    (len(self), ) + self.shape.
                image:
                    If present, an initial solution. If the dtype is correct, it
                    will also contain the final solution.
        """
        from numpy import zeros, max, product, ones, sqrt

        # Create missing weights and image objects if needed. Check they are
        # correct otherwise.
        dtype = visibility['y'].values.dtype
        if image is not None:
            if image.dtype != dtype: image = image.astype(dtype)
            if image.shape != self.image_size:
                msg = "Shape of input image should be %s" % self.image_size
                raise ValueError(msg)
        else: image = zeros(self.image_size, dtype=dtype, order='C')

        wshape = (len(self), ) + self.image_size
        if weights is not None:
            if weights.dtype != 'double': weights = weights.astype('double')
            if weights.size != wshape:
                raise ValueError('Weights should be of shape %s' % wshape)
        else: weights = ones(wshape, dtype='double')

        #
        measurements = MeasurementOperator( visibility, self.image_size,
                                            self.oversampling,
                                            self.interpolation )
        # Convert from python to C parameters
        cdef sopt_l1_sdmmparam params
        _convert_sdmm_params(&params, self, measurements, visibility)

        # define all C objects
        cdef double scale = sqrt(image.size) / sqrt(len(visibility))
        scaled_visibility = visibility['y'].values * scale
        cdef:
            void* c_image = untyped_pointer_to_data(image)
            void* c_visibility = untyped_pointer_to_data(scaled_visibility)
            void* c_weights = untyped_pointer_to_data(weights)
            void *c_wavelets
            _VoidedData forward_data = measurements.forward_voided_data(scale)
            _VoidedData adjoint_data = measurements.adjoint_voided_data(scale)

            int Nr = len(self) * image.size
            int Ny = len(visibility)
            void **datafwd = forward_data.data()
            void **dataadj = adjoint_data.data()
        params.gamma *= scale

        SparsityOperator.set_wavelet_pointer(self, &c_wavelets)

        sopt_l1_sdmm(
            c_image, image.size,
            &purify_measurement_cftfwd, datafwd,
            &purify_measurement_cftadj, dataadj,
            &sopt_sara_synthesisop, &c_wavelets,
            &sopt_sara_analysisop, &c_wavelets,
            Nr, c_visibility, Ny, <double*>c_weights, params
        )

        return image
