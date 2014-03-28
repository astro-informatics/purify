from . import params
from purify.measurements cimport purify_measurement_cftfwd, \
                                 purify_measurement_cftadj, \
                                 _VoidedData, MeasurementOperator
from purify.sparsity_ops cimport sopt_sara_analysisop, sopt_sara_synthesisop, \
                                 SparsityOperator
from purify.cparams cimport _convert_sdmm_params, _default_sigma,\
                            sopt_l1_sdmmparam
from purify.sdmm import SDMM

cdef extern from "sopt_l1.h":
    ctypedef struct sopt_l1_rwparam:
        int verbose
        int max_iter
        double rel_var
        double sigma
        int init_sol

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

cdef void _convert_rwparams( sopt_l1_rwparam *c_params, sdmm,
        MeasurementOperator measurements, visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[sdmm.rw.verbose]
    c_params.max_iter = sdmm.rw.max_iter
    c_params.rel_var = sdmm.rw.relative_variation
    c_params.init_sol = 1 if sdmm.rw.warm_start else 0
    if sdmm.rw.sigma is not None: c_params.sigma = sdmm.rw.sigma
    else:
        nelements = float(product(measurements.sizes.image) * len(sdmm))
        nvis = float(len(visibility['y']))
        sigma = _default_sigma(visibility['y'])
        c_params.sigma = sigma * sqrt(nvis / nelements)


class RWSDMM(SDMM):
    """ SDMM for the weighted L1 Problem

        Usage is similar to that :py:class:`SDMM`.
    """
    def __init__(self, rw_max_iter=5, rw_relative_variation=1e-3, **kwargs):
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
        from params import RW, pass_along
        super(SDMM, self).__init__(**kwargs)
        kwargs['rw_max_iter'] = rw_max_iter
        kwargs['rw_relative_variation'] = rw_relative_variation
        self.rw = RW(**pass_along('rw', **kwargs))
        """ Parameters related to the reweighting """

    @params.apply_params
    def __call__(self, visibility, image=None):
        """ Computes image from input visibility.

            :Parameters:
                visibility:
                    A pandas dataframe with a 'u', 'v', 'w',
                    'y' (visibility per se) columns
                image:
                    If present, an initial solution. If the dtype is correct, it
                    will also contain the final solution.
        """
        from numpy import zeros, max, product, ones, sqrt

        # Create missing weights and image objects if needed. Check they are
        # correct otherwise.
        dtype = visibility['y'].values.dtype
        warm_start = image is not None
        if warm_start:
            if image.dtype != dtype: image = image.astype(dtype)
            if image.shape != self.image_size:
                msg = "Shape of input image should be %s" % self.image_size
                raise ValueError(msg)
        else: image = zeros(self.image_size, dtype=dtype)

        #
        measurements = MeasurementOperator( visibility, self.image_size,
                                            self.oversampling,
                                            self.interpolation )
        # Convert from python to C parameters
        cdef sopt_l1_sdmmparam sdparams
        _convert_sdmm_params(&sdparams, self, measurements, visibility)
        cdef sopt_l1_rwparam rwparams
        _convert_rwparams(&rwparams, self, measurements, visibility)

        # define all C objects
        cdef:
            double scale = sqrt(image.size) / sqrt(len(visibility))
            unsigned char[::1] c_image = image.data
            void *c_wavelets
            unsigned char[::1] c_visibility \
                        = (visibility['y'].values * scale).data
            _VoidedData forward_data = measurements.forward_voided_data(scale)
            _VoidedData adjoint_data = measurements.adjoint_voided_data(scale)

            int Nr = len(self) * image.size
            int Ny = len(visibility)
            void **datafwd = forward_data.data()
            void **dataadj = adjoint_data.data()
            void *image_ptr = <void*> &c_image[0]
            void *visibility_ptr = <void*> &c_visibility[0]
        sdparams.gamma *= scale

        SparsityOperator.set_wavelet_pointer(self, &c_wavelets)

        sopt_l1_rwsdmm(
            image_ptr, image.size,
            &purify_measurement_cftfwd, datafwd,
            &purify_measurement_cftadj, dataadj,
            &sopt_sara_synthesisop, &c_wavelets,
            &sopt_sara_analysisop, &c_wavelets,
            Nr, visibility_ptr, Ny, sdparams, rwparams
        )

        return image
