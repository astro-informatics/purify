""" Defines parameters mix-in classes

    Parameters are defined as *traits*. This means that they can be set to only a subset of possible
    values, rather than just any python object. For instance, acceptable values for a tolerance
    criteria might be positive real numbers. Anything else should raise an expection. In a way,
    *traits* are akin to types in C. 

    The parameters are grouped in a way to reflect the C parameter structures.
"""
__all__ = ['ConjugateGradient', 'SDMM', 'RW', 'TVProx']
__docformat__ = "restructuredtext en"

def verbosity():
    """ Defines a verbosity property.

        There are three levels of verbosity, 'off', 'medium', and 'high'. 
    """
    doc = """ (off|0|None|False) | (summary|medium|1|True) | (convergence|high|2) """ 
    def get(self):
        return ['off', 'medium', 'high'][self._verbose]
    def set(self, value):
        if hasattr(value, 'lower'): value = value.lower()

        if value in [0, None, 'off', 'none']: value = 0
        elif value in [1, 'summary', 'medium']: value = 1
        elif value in [2, 'convergence', 'high']: value = 2
        else: raise ValueError("Verbose can be one of off, medium, high")
        self._verbose = value
    return property(get, set, doc=doc)

def _set_positive(self, value, name, cast):
    value = cast(value)
    if value <= 0: raise ValueError("%s cannot be negative or null" % name)
    setattr(self, "_%s" % name, value)

def positive_number(name, cast, doc=None):
    """ returns property for strictly positive numbers """
    return property(
        lambda x: getattr(x, "_%s" % name),
        lambda x, v: _set_positive(x, v, name, cast),
        doc
    )
def positive_real(name, doc=None):
    """ Parameter must be a strictly positive real number """
    return positive_number(name, float, doc)
def positive_int(name, doc=None):
    """ Parameter must be a strictly positive integer number """
    return positive_number(name, int, doc)
def positive_real_or_none(name, doc):
    """ A property that can be a positive real number, or None

        None can be used to indicate the property should be computed somehow.
    """
    _name = "_%s" % name
    return property(
        lambda x: getattr(x, _name),
        lambda x, v: setattr(x, _name, None) if v is None else _set_positive(x, v, name, float),
        doc
    )

def boolean(name, doc=None):
    """ A boolean property """
    return property(
        lambda x: getattr(x, "_%s" % name),
        lambda x, v: setattr(x, "_%s" % name, True if v else False), 
        doc = doc
    )

class ConjugateGradient(object):
    """ Contains conjugate gradient parameters. """
    max_iter = positive_int('max_iter', "Maximum number of Conjugate-gradient iterations")
    tolerance = positive_real('radius', "Conjugate gradient convergence criteria")

    def __init__(self, max_iter=100, tolerance=1e-6):
        self.max_iter = max_iter
        self.tolerance = tolerance

class TVProx(object):
    verbose = verbosity()
    max_iter = positive_int('max_iter', "Maximum number of iterations")
    relative_variation = positive_real( 'relative_variation',
                                        "Acceptable relative variation in the solution" )
    def __init__(self, verbose='high', max_iter=50, relative_variation=1e-4, **kwargs):
        super(TVProx, self).__init__()
        self.verbose = verbose
        self.max_iter = max_iter
        self.relative_variation = relative_variation


class SDMM(TVProx):
    """ SDMM related parameters """

    gamma = positive_real_or_none('gamma', "SDMM convergence criteria")
    radius = positive_real_or_none('radius', "L2 ball radius")
    relative_radius = positive_real('relative_radius', "Defines range of the radius")

    def __init__( self, verbose='high', max_iter=300, gamma=None, relative_variation=1e-3,
                  radius=None, relative_radius=1e-2, cg_max_iter=100, cg_tolerance=1e-6,
                  **kwargs ):
        super(SDMM, self).__init__( verbose=verbose, max_iter=max_iter,
                                    relative_variation=relative_variation )
        self.gamma = gamma
        self.radius = radius
        self.relative_radius = relative_radius
        self.cg = ConjugateGradient(max_iter=cg_max_iter, tolerance=cg_tolerance)
        """ Conjugate gradient parameters """

class RW(TVProx):
    """ Reweighted L1 optimization problem """

    sigma = positive_real_or_none( 'sigma', 
                                   "Standard deviation of the noise in the representation domain" )
    warm_start = boolean('warm_start', "If True, use provided starting point")

    def __init__( self, verbose='high', max_iter=5, sigma=None, relative_variation=1e-3,
                  warm_start=True, **kwargs ):
        super(RW, self).__init__( verbose=verbose, max_iter=max_iter,
                                  relative_variation=relative_variation )
        self.sigma = sigma
        self.warm_start = warm_start


class TV_SDMM(SDMM):
    """ Parameters for the TV optimisation problem using SDMM """
    def __init__( self, verbose='high', max_iter=300, gamma=None, relative_variation=1e-3,
                  radius=None, relative_radius=1e-2, cg_max_iter=100, cg_tolerance=1e-6,
                  tv_verbose='high', tv_max_iter=50, tv_relative_variation=1e-4, **kwargs):
        super(TV_SDMM, self).__init__( verbose=verbose, max_iter=max_iter, gamma=gamma,
                                       relative_variation=relative_variation, radius=radius,
                                       relative_radius=relative_radius, cg_max_iter=cg_max_iter,
                                       cg_tolerance=cg_tolerance )
        self.tv = TVProx( verbose=tv_verbose, max_iter=tv_max_iter,
                          relative_variation=tv_relative_variation )
