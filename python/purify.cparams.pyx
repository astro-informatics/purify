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

cdef void _convert_prox_tvparam(sopt_prox_tvparam* c_params, pyinput):
    """ Sets c parameters from python object """
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[pyinput.verbose]
    c_params.max_iter = pyinput.max_iter
    c_params.rel_obj = pyinput.relative_variation

cdef void _convert_l1param( sopt_l1_sdmmparam* c_params, sdmm,
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
    _convert_l1param(<sopt_l1_sdmmparam*>c_params, sdmm, sensing_op,
                    visibility)
    _convert_prox_tvparam(<sopt_prox_tvparam*>&(c_params.paramtv), sdmm.tv)
