from purify.sensing cimport purify_measurement_cftfwd, \
                            purify_measurement_cftadj, \
                            SensingOperator, _VoidedData

cdef extern from "purify_measurement.h":
    double purify_measurement_pow_meth(
        void (*A)(void *out, void *invec, void **data),
        void **A_data,
        void (*At)(void *out, void *invec, void **data),
        void **At_data);

cdef double _default_sigma(visibilities) except *:
    from numpy import sqrt
    from numpy.linalg import norm
    nvis = float(len(visibilities))
    return norm(visibilities) * 10.0**-(1.5) / sqrt(nvis)

cdef double guess_gamma_penalty(
        sdmm, visibility, SensingOperator sensing_op ) except *:
    """ Evaluates the convergence criteria from the input """
    from numpy import max, product, sqrt
    xout = sensing_op.adjoint(visibility)
    cdef double nvis = float(len(visibility))
    cdef double npixels = product(sensing_op.sizes.image)
    cdef double scale = sqrt(npixels / nvis)
    cdef:
        double gamma = max((xout if sdmm.tv_norm else sdmm.analyze(xout)).real)
    return  gamma * 1e-3 * scale

cdef double guess_radius(
        sdmm, visibility, SensingOperator sensing_op) except *:
    from numpy import product, sqrt
    cdef double nvis = float(len(visibility))
    cdef double npixels = product(sensing_op.sizes.image)
    cdef double scale = sqrt(npixels / nvis)
    cdef double sigma = _default_sigma(visibility)
    return sqrt(nvis + 2.0 * sqrt(nvis)) * sigma

cdef double guess_sigma(sdmm, visibility, SensingOperator sensing_op) except *:
    from numpy import product, sqrt
    cdef double nvis = float(len(visibility))
    cdef double npixels = product(sensing_op.sizes.image)
    return _default_sigma(visibility) \
            * nvis / npixels / sqrt(len(float(sdmm)))

cdef double guess_nu(SensingOperator sensing_op) except *:
    cdef:
        _VoidedData forward_data = sensing_op.forward_voided_data(None)
        _VoidedData adjoint_data = sensing_op.adjoint_voided_data(None)

        void **datafwd = forward_data.data()
        void **dataadj = adjoint_data.data()

    return purify_measurement_pow_meth(
          &purify_measurement_cftfwd, datafwd,
          &purify_measurement_cftadj, dataadj)

cdef void _convert_prox_tvparam(sopt_prox_tvparam* c_params, pyinput):
    """ Sets c parameters from python object """
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[pyinput.verbose]
    c_params.max_iter = pyinput.max_iter
    c_params.rel_obj = pyinput.relative_variation

cdef void _convert_prox_l1_params(sopt_prox_l1param* c_params, pyinput):
    """ Sets c parameters from python object """
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, pyinput)
    c_params.nu = pyinput.nu
    c_params.tight = 1 if pyinput.tight_frame else 0
    c_params.pos = 1 if pyinput.positivity else 0

cdef void _convert_l1_padmm_param(
        sopt_l1_param_padmm* c_params, padmm,
        SensingOperator sensing_op, visibility) except *:
    """ Sets c parameters from python object """
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[padmm.verbose]
    c_params.max_iter = padmm.max_iter
    c_params.rel_obj = padmm.relative_variation

    c_params.gamma = padmm.gamma if padmm.gamma is not None \
        else guess_gamma_penalty(padmm, visibility, sensing_op)
    c_params.epsilon = padmm.radius if padmm.radius is not None \
        else guess_radius(padmm, visibility, sensing_op)

    c_params.real_out = 1;
    c_params.real_meas = 0;
    _convert_prox_l1_params(&c_params.paraml1, padmm.l1)
    c_params.epsilon_tol_scale = padmm.epsilon_tol_scale
    c_params.lagrange_update_scale = padmm.lagrange_update_scale
    c_params.nu = guess_nu(sensing_op)


cdef void _convert_l1_sdmm_param( sopt_l1_sdmmparam* c_params, sdmm,
                                  SensingOperator sensing_op,
                                  visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    if visibility.dtype != 'complex':
        raise NotImplemented('Cannot purify real visibilities')

    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm)

    c_params.gamma = sdmm.gamma if sdmm.gamma is not None \
        else guess_gamma_penalty(sdmm, visibility, sensing_op)
    c_params.epsilon = sdmm.radius if sdmm.radius is not None \
        else guess_radius(sdmm, visibility, sensing_op)
    c_params.epsilon_tol = sdmm.relative_radius
    c_params.cg_max_iter = sdmm.cg.max_iter
    c_params.cg_tol      = sdmm.cg.tolerance
    c_params.real_data   = 0

cdef void _convert_rwparams( sopt_l1_rwparam *c_params, sdmm,
        SensingOperator sensing_op, visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm.rw)
    c_params.init_sol = 1 if sdmm.rw.warm_start else 0
    if sdmm.rw.sigma is not None: c_params.sigma = sdmm.rw.sigma
    elif c_params.init_sol == 1: c_params.sigma = 0
    else: c_params.sigma = guess_sigma(sdmm, visibility, sensing_op)

cdef void _convert_tvparam( sopt_tv_sdmmparam* c_params, sdmm,
                            SensingOperator sensing_op,
                            visibility ) except *:
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm)
    c_params.rel_obj = c_params.gamma # Order changes depending on structure
    _convert_l1_sdmm_param(<sopt_l1_sdmmparam*>c_params, sdmm, sensing_op,
                           visibility)
    _convert_prox_tvparam(<sopt_prox_tvparam*>&(c_params.paramtv), sdmm.tv)
