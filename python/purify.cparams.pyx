cdef double _default_sigma(measurements) except *:
    from numpy import sqrt
    from numpy.linalg import norm
    nvis = float(len(measurements))
    return norm(measurements) * 10.0**-(1.5) / sqrt(nvis)

cdef void _convert_prox_tvparam(sopt_prox_tvparam* c_params, pyinput):
    """ Sets c parameters from python object """
    c_params.verbose = {'off': 0, 'medium': 1, 'high': 2}[pyinput.verbose]
    c_params.max_iter = pyinput.max_iter
    c_params.rel_obj = pyinput.relative_variation

cdef void _convert_l1param( sopt_l1_sdmmparam* c_params, sdmm,
                            MeasurementOperator measurements,
                            visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm)
    c_params.rel_obj = c_params.gamma # Order changes depending on structure
    nelements = product(measurements.sizes.image)
    if sdmm.gamma is not None: c_params.gamma = sdmm.gamma
    else:
        scale = sqrt(nelements) / sqrt(float(len(visibility)))
        xout = measurements.adjoint(visibility['y'].values)
        if sdmm.tv_norm: c_params.gamma = 1e-3 * max(xout.real) * scale * scale
        else: c_params.gamma = 1e-3 * max(sdmm.analyze(xout).real) * scale * scale
    if sdmm.radius is not None: c_params.epsilon = sdmm.radius
    else:
        nvis = float(len(visibility))
        sigma = _default_sigma(visibility['y'])
        c_params.epsilon = sqrt(nvis + 2.0 * sqrt(nvis)) * sigma \
                           / nvis * nelements

    c_params.epsilon_tol = sdmm.relative_radius
    c_params.cg_max_iter = sdmm.cg.max_iter
    c_params.cg_tol = sdmm.cg.tolerance
    if visibility['y'].values.dtype != 'complex':
        raise NotImplemented('Cannot purify real visibilities')
    c_params.real_data = 0

cdef void _convert_rwparams( sopt_l1_rwparam *c_params, sdmm,
        MeasurementOperator measurements, visibility ) except *:
    """ Sets c parameters from python object """
    from numpy import max, product, sqrt
    from numpy.linalg import norm
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm.rw)
    c_params.init_sol = 1 if sdmm.rw.warm_start else 0
    if sdmm.rw.sigma is not None: c_params.sigma = sdmm.rw.sigma
    elif c_params.init_sol == 1: c_params.sigma = 0
    else:
        nelements = float(product(measurements.sizes.image) * len(sdmm))
        nvis = float(len(visibility['y']))
        sigma = _default_sigma(visibility['y'])
        c_params.sigma = sigma * sqrt(nvis / nelements)

cdef void _convert_tvparam( sopt_tv_sdmmparam* c_params, sdmm,
                            MeasurementOperator measurements,
                            visibility ) except *:
    _convert_prox_tvparam(<sopt_prox_tvparam*>c_params, sdmm)
    c_params.rel_obj = c_params.gamma # Order changes depending on structure
    _convert_l1param(<sopt_l1_sdmmparam*>c_params, sdmm, measurements,
                    visibility)
    _convert_prox_tvparam(<sopt_prox_tvparam*>&(c_params.paramtv), sdmm.tv)
