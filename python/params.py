""" Defines parameters mix-in classes

    Parameters are defined as *traits*. This means that they can be set to only
    a subset of possible values, rather than just any python object. For
    instance, acceptable values for a tolerance criteria might be positive real
    numbers. Anything else should raise an expection. In a way, *traits* are
    akin to types in C.

    The parameters are grouped in a way to reflect the C parameter structures.
"""
__all__ = ['ConjugateGradient', 'SDMM', 'RW', 'TVProx', 'Measurements',
           'apply_params']
__docformat__ = "restructuredtext en"

def _trait_name(name):
    """ Defines name across all traits. """
    return "_trait_%s" % name
def verbosity():
    """ Defines a verbosity property.

        There are three levels of verbosity, 'off', 'medium', and 'high'.
    """
    doc = "(off|0|None|False) | (summary|medium|1|True) | (convergence|high|2)"
    _name = _trait_name('verbose')
    def get(self):
        return ['off', 'medium', 'high'][getattr(self, _name)]
    def set(self, value):
        if hasattr(value, 'lower'): value = value.lower()

        if value in [0, None, 'off', 'none']: value = 0
        elif value in [1, 'summary', 'medium']: value = 1
        elif value in [2, 'convergence', 'high']: value = 2
        else: raise ValueError("Verbose can be one of off, medium, high")
        setattr(self, _name, value)
    return property(get, set, doc=doc)

def _set_positive(self, value, name, cast):
    value = cast(value)
    if value <= 0: raise ValueError("%s cannot be negative or null" % name)
    setattr(self, _trait_name(name), value)

def positive_number(name, cast, doc=None):
    """ returns property for strictly positive numbers """
    return property(
        lambda x: getattr(x, _trait_name(name)),
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
    _name = _trait_name(name)
    get = lambda x: getattr(x, _name)
    set = lambda x, v: setattr(x, _name, None) if v is None \
                       else _set_positive(x, v, name, float)
    return property(get, set, doc=doc)

def boolean(name, doc=None):
    """ A boolean property """
    get = lambda x: getattr(x, _trait_name(name))
    set = lambda x, v: setattr(x, _trait_name(name), True if v else False)
    return property(get, set, doc=doc)

def size_property(name, doc=None):
    """ A property that sets 2d sizes. """
    from collections import namedtuple
    _name = _trait_name(name)
    Size = namedtuple('Size', ['x', 'y'])
    def set(self, value):
        x, y = int(value[0]), int(value[1])
        if x <= 0 or y <= 0: raise ValueError("Size must be positive.")
        setattr(self, _name, Size(x, y))
    return property(lambda x: getattr(x, _name), set, doc=doc)


class ConjugateGradient(object):
    """ Contains conjugate gradient parameters. """
    max_iter = positive_int('max_iter',
            "Maximum number of Conjugate-gradient iterations")
    tolerance = positive_real('radius',
            "Conjugate gradient convergence criteria")

    def __init__(self, max_iter=100, tolerance=1e-6, **kwargs):
        super(ConjugateGradient, self).__init__(**kwargs)
        self.max_iter = max_iter
        self.tolerance = tolerance

class TVProx(object):
    verbose = verbosity()
    max_iter = positive_int('max_iter', "Maximum number of iterations")
    relative_variation = positive_real( 'relative_variation',
                   "Acceptable relative variation in the solution" )
    def __init__(self, verbose='high', max_iter=300, relative_variation=1e-4,
                 **kwargs):
        super(TVProx, self).__init__()
        self.verbose = verbose
        self.max_iter = max_iter
        self.relative_variation = relative_variation


class SDMM(TVProx):
    """ SDMM related parameters """
    gamma = positive_real_or_none('gamma', "SDMM convergence criteria")
    radius = positive_real_or_none('radius', "L2 ball radius")
    relative_radius = positive_real('relative_radius',
            "Defines range of the radius")

    def __init__( self, gamma=None, radius=None, relative_radius=1e-2,
                  cg_max_iter=100, cg_tolerance=1e-6, **kwargs ):
        super(SDMM, self).__init__(**kwargs)
        self.gamma = gamma
        self.radius = radius
        self.relative_radius = relative_radius
        self.cg = ConjugateGradient(max_iter=cg_max_iter, tolerance=cg_tolerance)
        """ Conjugate gradient parameters """

class RW(TVProx):
    """ Reweighted L1 optimization problem """
    sigma = positive_real_or_none('sigma',
          "Standard deviation of the noise in the representation domain" )
    warm_start = boolean('warm_start',
            "Whether to use initial solution or make a guess")

    def __init__(self, sigma=None, warm_start=True, **kwargs):
        super(RW, self).__init__(**kwargs)
        self.sigma = sigma
        self.warm_start = warm_start


class TV_SDMM(SDMM):
    """ Parameters for the TV optimisation problem using SDMM """
    def __init__(self, tv_verbose='high', tv_max_iter=50,
                 tv_relative_variation=1e-4, **kwargs):
        super(TV_SDMM, self).__init__(**kwargs)
        self.tv = TVProx( verbose=tv_verbose, max_iter=tv_max_iter,
                          relative_variation=tv_relative_variation )
class Measurements(object):
    """ Parameter entering measurement and output values. """
    image_size = size_property("image_size", "Output image size")
    interpolation = size_property("interpolation",
            "Size of the interpolation kernel")
    oversampling = size_property("oversampling", "Oversampling factor")
    def __init__( self, image_size=(256, 256), oversampling=(2,2),
                  interpolation=(24, 24), **kwargs ):
        super(Measurements, self).__init__(**kwargs)

        self.image_size = image_size
        self.oversampling = oversampling
        self.interpolation = interpolation

def pass_along(name, **kwargs):
    """ Computes parameter dictionaries for inner attributes.

        Returns a dictionary containing key/value pairs with the following transformation:

        - key matches "%s_(.*)" % name: add {'\\1': value}
        - key does not mathc "%s_(.*)", drop value
    """
    result = {}
    start = "%s_" % name
    for key, value in kwargs.iteritems():
        if len(key) > len(start) and key[:len(start)] == start:
            result[key[len(start):]] = value
    return result

def apply_params(inner):
    """ Modify traits using keyword arguments for duration of call.

        The traits are modified for the duration of the call only.
    """
    from functools import wraps
    @wraps(inner)
    def modified_self(self, *args, **kwargs):
        saved_traits, other_kwargs = {}, {}
        try:
            # Save and modify traits that appear in kwargs
            for name, value in kwargs.iteritems():
                attr = _trait_name(name)
                if hasattr(self, attr):
                    saved_traits[name] = getattr(self, attr)
                    setattr(self, name, value)
                else: other_kwargs[name] = value
            return inner(self, *args, **other_kwargs)
        finally:
            for name, value in saved_traits.iteritems():
                setattr(self, _trait_name(name), value)
    return modified_self
