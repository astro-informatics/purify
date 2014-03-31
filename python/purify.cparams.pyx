cdef double _default_sigma(measurements) except *:
    from numpy import norm, sqrt
    nvis = float(len(measurements))
    return norm(measurements) * 10.0**-(1.5) / sqrt(nvis)

cdef void _convert_sdmm_params( sopt_l1_sdmmparam* c_params, sdmm,
                                MeasurementOperator measurements,
                                visibility ) except *:
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

