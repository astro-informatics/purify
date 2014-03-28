from . import params
from purify.measurements cimport purify_measurement_cftfwd, \
                                 purify_measurement_cftadj, \
                                 MeasurementOperator, _VoidedData
from purify.sparsity_ops cimport sopt_sara_analysisop, sopt_sara_synthesisop, \
                                 SparsityOperator
from purify.cparams cimport sopt_l1_sdmmparam, _convert_sdmm_params

cdef extern from "sopt_l1.h":
    cdef void sopt_l1_sdmm( void *xsol, int nx,
            void (*A)(void *out, void *invec, void **data),
            void **A_data, void (*At)(void *out, void *invec, void **data),
            void **At_data, void (*Psi)(void *out, void *invec, void **data),
            void **Psi_data, void (*Psit)(void *out, void *invec, void **data),
            void **Psit_data, int nr, void *y, int ny, double *weights,
            sopt_l1_sdmmparam param ) nogil

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
        else: image = zeros(self.image_size, dtype=dtype)

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
        cdef sopt_l1_sdmmparam sdparams
        _convert_sdmm_params(&sdparams, self, measurements, visibility)

        # define all C objects
        cdef:
            double scale = sqrt(image.size) / sqrt(len(visibility))
            unsigned char[::1] c_image = image.data
            void *c_wavelets
            unsigned char[::1] c_visibility \
                        = (visibility['y'].values * scale).data
            unsigned char[::1] c_weights = weights.data
            _VoidedData forward_data = measurements.forward_voided_data(scale)
            _VoidedData adjoint_data = measurements.adjoint_voided_data(scale)

            int Nr = len(self) * image.size
            int Ny = len(visibility)
            void **datafwd = forward_data.data()
            void **dataadj = adjoint_data.data()
            void *image_ptr = <void*> &c_image[0]
            void *visibility_ptr = <void*> &c_visibility[0]
            double *weights_ptr = <double*> &c_weights[0]
        sdparams.gamma *= scale

        SparsityOperator.set_wavelet_pointer(self, &c_wavelets)

        sopt_l1_sdmm(
            image_ptr, image.size,
            &purify_measurement_cftfwd, datafwd,
            &purify_measurement_cftadj, dataadj,
            &sopt_sara_synthesisop, &c_wavelets,
            &sopt_sara_analysisop, &c_wavelets,
            Nr, visibility_ptr, Ny, weights_ptr, sdparams
        )

        return image
