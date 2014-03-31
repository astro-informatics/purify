from . import params
from purify.measurements cimport purify_measurement_cftfwd, \
                                 purify_measurement_cftadj, \
                                 MeasurementOperator, _VoidedData
from purify.sparsity_ops cimport sopt_sara_analysisop, sopt_sara_synthesisop, \
                                 SparsityOperator
from purify.cparams cimport sopt_l1_sdmmparam, _convert_sdmm_params, \
                            sopt_l1_rwparam, _convert_rwparams

cdef extern from "sopt_l1.h":
    cdef void sopt_l1_sdmm( void *xsol, int nx,
            void (*A)(void *out, void *invec, void **data),
            void **A_data, void (*At)(void *out, void *invec, void **data),
            void **At_data, void (*Psi)(void *out, void *invec, void **data),
            void **Psi_data, void (*Psit)(void *out, void *invec, void **data),
            void **Psit_data, int nr, void *y, int ny, double *weights,
            sopt_l1_sdmmparam param ) nogil

    cdef void sopt_l1_rwsdmm(void *xsol, int nx,
        void (*A)(void *out, void *into, void **data),
        void **A_data,
        void (*At)(void *out, void *into, void **data),
        void **At_data,
        void (*Psi)(void *out, void *into, void **data),
        void **Psi_data,
        void (*Psit)(void *out, void *into, void **data),
        void **Psit_data,
        int nr,
        void *y,
        int ny,
        sopt_l1_sdmmparam paraml1,
        sopt_l1_rwparam paramrwl1)

cdef void _l1_sdmm( self, measurements, visibility,
                    unsigned char[::1] _image, int _image_size,
                    void **_datafwd, void **_dataadj,
                    unsigned char[::1] _visibility,
                    int _nmeasurements, weights,  double scale ) except *:
    """ Calls L1 SDMM """
    cdef:
        sopt_l1_sdmmparam sdparams
        void *c_wavelets
        unsigned char[::1] c_weights = weights.data
        double *weights_ptr = <double*> &c_weights[0]
        int Nr = len(self) * _image_size
    _convert_sdmm_params(&sdparams, self, measurements, visibility)
    sdparams.gamma *= scale
    SparsityOperator.set_wavelet_pointer(self, &c_wavelets)

    sopt_l1_sdmm(
        <void*>&_image[0], _image_size,
        &purify_measurement_cftfwd, _datafwd,
        &purify_measurement_cftadj, _dataadj,
        &sopt_sara_synthesisop, &c_wavelets,
        &sopt_sara_analysisop, &c_wavelets,
        Nr, <void *> &_visibility[0], _nmeasurements,
        weights_ptr, sdparams
    )

cdef void _l1_rw_sdmm( self, measurements, visibility,
                       unsigned char[::1] _image, int _image_size,
                       void **_datafwd, void **_dataadj,
                       unsigned char[::1] _visibility,
                       int _nmeasurements, double scale ) except *:
    """ Calls L1 SDMM """
    cdef:
        sopt_l1_sdmmparam sdparams
        sopt_l1_rwparam rwparams
        void *c_wavelets
        int Nr = len(self) * _image_size
    _convert_sdmm_params(&sdparams, self, measurements, visibility)
    _convert_rwparams(&rwparams, self, measurements, visibility)
    sdparams.gamma *= scale
    SparsityOperator.set_wavelet_pointer(self, &c_wavelets)

    sopt_l1_rwsdmm(
        <void*>&_image[0], _image_size,
        &purify_measurement_cftfwd, _datafwd,
        &purify_measurement_cftadj, _dataadj,
        &sopt_sara_synthesisop, &c_wavelets,
        &sopt_sara_analysisop, &c_wavelets,
        Nr, <void *> &_visibility[0], _nmeasurements,
        sdparams, rwparams
    )

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
    reweighted = params.boolean("reweighted", "Whether to apply the reweighted scheme")
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
                reweighted: boolean
                    If True, applies the reweighted scheme
                rw_max_iter: int
                    Maximum number of iterations for the global RW-L1 problem.
                    Defaults to 5.
                rw_verbose: off|medium|high
                    Verbosity when solving the global RW-L1 problem.
                    Defaults to high.
                rw_sigma: float
                    Noise standard deviation in the representation domain.
                    Defaults to None, in which it is computed internally from the input
                    visibilities.
                rw_relative_variation: float
                    Convergence criteria for the L1 problem.
                    Minimum relative variation in the solution.
                    Defaults to 1e-3.
        """
        super(SDMM, self).__init__(**kwargs)
        if 'rw_max_iter' not in kwargs: kwargs['rw_max_iter'] = 5
        if 'rw_relative_variation' not in kwargs:
            kwargs['rw_relative_variation'] = 5
        self.rw = params.RW(**params.pass_along('rw', **kwargs))
        """ Parameters related to the reweighting """
        self.reweighted = kwargs.pop('reweighted', False) == True

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
        image = self._get_image(image, visibility['y'].values.dtype)
        if not self.reweighted: weights = self._get_weight(weights)
        elif weights is not None:
            msg = "weights input are not meaningfull in reweighted scheme"
            raise ValueError(msg)

        #
        measurements = MeasurementOperator( visibility, self.image_size,
                                            self.oversampling,
                                            self.interpolation )
        # define all common C objects
        cdef:
            double scale = sqrt(image.size) / sqrt(len(visibility))
            _VoidedData forward_data = measurements.forward_voided_data(scale)
            _VoidedData adjoint_data = measurements.adjoint_voided_data(scale)

            int Nr = len(self) * image.size
            int Ny = len(visibility)
            void **datafwd = forward_data.data()
            void **dataadj = adjoint_data.data()

        if not self.reweighted:
            _l1_sdmm( self, measurements, visibility,
                      image.data, image.size,
                      datafwd, dataadj,
                      (visibility['y'].values * scale).data, Ny,
                      weights, scale )
        else:
            _l1_rw_sdmm( self, measurements, visibility,
                         image.data, image.size,
                         datafwd, dataadj,
                         (visibility['y'].values * scale).data, Ny,
                         scale)

        return image


    def _get_image(self, image, dtype):
        """ Check/create input image """
        from numpy import zeros
        if image is not None:
            if image.dtype != dtype: image = image.astype(dtype)
            if image.shape != self.image_size:
                msg = "Shape of input image should be %s" % self.image_size
                raise ValueError(msg)
        else: image = zeros(self.image_size, dtype=dtype)
        return image

    def _get_weight(self, weights):
        """ Check/create input weights """
        from numpy import ones
        wshape = (len(self), ) + self.image_size
        if weights is not None:
            if weights.dtype != 'double': weights = weights.astype('double')
            if weights.size != wshape:
                raise ValueError('Weights should be of shape %s' % wshape)
        else: weights = ones(wshape, dtype='double')
        return weights
