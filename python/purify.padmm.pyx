from .numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from . import params
from .base import ProximalMinimizationBase
from .sensing cimport purify_measurement_cftfwd, purify_measurement_cftadj, \
                      SensingOperator, _VoidedData
from .cparams cimport sopt_l1_param_padmm, _convert_l1_padmm_param
from .sparsity_ops cimport sopt_sara_analysisop, sopt_sara_synthesisop, \
                           SparsityOperator

cdef extern from "sopt_l1.h":
    void sopt_l1_solver_padmm(void *xsol, int nx,
        void (*A)(void *out, void *vecin, void **data), void **A_data,
        void (*At)(void *out, void *vecin, void **data), void **At_data,
        void (*Psi)(void *out, void *vecin, void **data), void **Psi_data,
        void (*Psit)(void *out, void *vecin, void **data), void **Psit_data,
        int nr, void *y, int ny, double *weights_l1, double *weights_l2,
        sopt_l1_param_padmm param)

class PADMM(params.Measurements, params.PADMM, SparsityOperator,
            ProximalMinimizationBase):
    """ Performs Alternating Direction Method of Mulitpliers

        Once instantiated for a give output image size and a given input set
        wavelet basis, this function can be used to recover the image for any
        number of visibility data sets:

            >>> from purify import PADMM, read_visibility
            >>> padmm = PADMM(...) # see below
            >>> visibility = read_visibility("file.vis")
            >>> image = padmm(visibility)

        There are a fair number of parameters, from the size of the output
        image, to the type of wavelet basis sets that should be used to
        reconstruct it. Please see PADMM.__init__ for details. After
        instanciation, most of these parameters can be accessed and modified as
        attributes of the function object.
    """
    def __init__(self, **kwargs):
        """ Creates a PADMM function object

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
                max_iter: int
                    Maximum number of PADMM iterations. Defaults to 300.
                relative_variation: float
                    Convergence criteria against changes in the objective
                    function. Defaults to 1e-4.
                verbose: off|medium|high
                    String indicating the level of verbosity. Defaults to
                    'high'.
                epsilon__tol_scale:
                    Scale tolerance on epsilon
                lagrange_update_scale:
                    Scale parameter when updating lagrange multiplier
                nu:
                    Measurement operator norm squared
                l1_max_iter: int
                    Maximum number of iterations for l1 proximal computation.
                    Defaults to 300.
                l1_positivity: bool
                    Whether to include positivity constraints.
                    Defaults to True.
                l1_relative_variation: float
                    Convergence criteria against changes in the objective
                    function during L1 proximal computations. Defaults to 1e-4.
                l1_thight_frame: bool
                    Whether Psi^T is a tight frame. Defaults to True.
                l1_verbose: off|medium|high
                    Verbosity of L1 proximal computations. Defaults to 'high'.
                l1_nu: float
                    Bounds on the squared norm of the Psi operator.
                    Defaults to 1.0.
        """
        l1 = kwargs.pop('l1', {})
        for key in [u for u in kwargs.keys()]:
            if len(key) > 3 and key[:3] == "l1_":
                l1[key[3:]] = kwargs.pop(key)
        super(PADMM, self).__init__(l1=l1, **kwargs)

    @params.apply_params
    def __call__(self, visibility, weights=None, image=None, scale='default'):
        """ Computes image from input visibility.

            :Parameters:
                visibility:
                    U, V, Y, (optionally L2 weights), and visibility data, in
                    one of the following formats:

                    - a pandas dataframe with a 'u', 'v', 'y' (visibility per
                      se) columns [+ 'w' optionally]
                    - a dictionary with 'u', 'v', and 'y' keys [+ 'w'
                    optionally]
                    - a seqence of three numpy arrays in the order 'u', 'v',
                      'y' [optional fourth array is 'w' the L2 weights]

                weights:
                    If present, an array of weights of shape
                    (len(self), ) + self.shape.
                scale:
                    Scaling factor applied to the visibility (and the
                    deconvolution kernel).

                    - If 'auto' or 'default', it is set to
                      :math:`\sqrt(l_p)/\sqrt(l_y)` where :math:`l_p` is the
                      number of pixels in the image and :math:`l_y` the number
                      of visibility measurements.
                    - If 'None', 'none', or None, then no scaling is applied
                    - Otherwise, it should be a number

                image:
                    If present, an initial solution. If the dtype is correct,
                    it will also contain the final solution, in order to avoid
                    unnecessary memory allocation. However, this means that
                    image may or may not be modified depending on how it is
                    chosen.
        """
        from numpy import ones, iscomplexobj

        scaled_y, weights_l2, image, scale \
                = self._normalize_input(visibility, scale, image, "float64")
        weights = self._get_weight(weights)
        if weights_l2 is None:
            weights_l2 = ones(scaled_y.shape, dtype=weights.dtype)

        cdef:
            # Defined as C variable so that it's c member functions are
            # accesible
            SensingOperator sensing_op = self.sensing_operator(visibility)
            _VoidedData forward_data = sensing_op.forward_voided_data(scale)
            _VoidedData adjoint_data = sensing_op.adjoint_voided_data(scale)

            void **datafwd = forward_data.data()
            void **dataadj = adjoint_data.data()

        cdef:
            sopt_l1_param_padmm params
            void* c_wavelets
            double* c_weights = <double*>untyped_pointer_to_data(weights)
            double* c_weights_l2 = <double*>untyped_pointer_to_data(weights_l2)
            void* c_image = untyped_pointer_to_data(image)
            void* c_visibility = untyped_pointer_to_data(scaled_y)
            int Nr = len(self) * image.size

        _convert_l1_padmm_param(&params, self, sensing_op, scaled_y)
        params.real_out = 1
        params.real_meas = 0 if iscomplexobj(scaled_y) else 1
        SparsityOperator.set_wavelet_pointer(self, &c_wavelets,
                                             params.real_out)

        sopt_l1_solver_padmm(
            c_image, image.size,
            &purify_measurement_cftfwd, datafwd,
            &purify_measurement_cftadj, dataadj,
            &sopt_sara_synthesisop, &c_wavelets,
            &sopt_sara_analysisop, &c_wavelets,
            Nr, c_visibility, len(scaled_y),
            c_weights, c_weights_l2, params
        )
        return image
