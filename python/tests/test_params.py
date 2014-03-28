""" Test parameters """
def assert_positive_integer(object, name):
    from nose.tools import assert_equal, assert_raises

    setattr(object, name, 2)
    assert_equal(getattr(object, name), 2)

    setattr(object, name, 1.4)
    assert_equal(getattr(object, name), 1)

    with assert_raises(ValueError): setattr(object, name, 0)
    with assert_raises(ValueError): setattr(object, name, -1)

def assert_positive_real(object, name):
    from nose.tools import assert_almost_equal, assert_raises

    setattr(object, name, 2)
    assert_almost_equal(getattr(object, name), 2)

    setattr(object, name, 1e-2)
    assert_almost_equal(getattr(object, name), 1e-2)

    with assert_raises(ValueError): setattr(object, name, 0)
    with assert_raises(ValueError): setattr(object, name, -1)

def assert_positive_real_or_none(object, name):
    from nose.tools import assert_true

    assert_positive_real(object, name)
    setattr(object, name, None)
    assert_true(getattr(object, name) is None)

def assert_verbosity(param):
    from nose.tools import assert_equal, assert_raises
    for key, values in { 'off': [0, None, 'oFf', False],
                         'medium': [1, 'meDium', 'sUMMary', True],
                         'high': [2, 'high', 'convergence'] }.iteritems():
        for value in values:
            param.verbose = value
            assert_equal(param.verbose, key)

    with assert_raises(ValueError): param.verbose = 'med'
    with assert_raises(ValueError): param.verbose = 0.1

def assert_boolean(param, name):
    from nose.tools import assert_true

    setattr(param, name, True)
    assert_true(getattr(param, name) is True)
    setattr(param, name, "not empty")
    assert_true(getattr(param, name) is True)

    setattr(param, name, 0)
    assert_true(getattr(param, name) is False)
    setattr(param, name, "")
    assert_true(getattr(param, name) is False)

def assert_size(param, name):
    from nose.tools import assert_equal, assert_raises
    setattr(param, name, (50, 51))
    assert_equal(getattr(param, name), (50, 51))
    assert_equal(getattr(getattr(param, name), 'x'), 50)
    assert_equal(getattr(getattr(param, name), 'y'), 51)
    for value in [(0, 1), (1, 0), (-1, 1), (1, -1), (0, 0)]:
       with assert_raises(ValueError): setattr(param, name, value)

def assert_tvprox(param):
    assert_verbosity(param)
    assert_positive_integer(param, 'max_iter')
    assert_positive_real(param, 'relative_variation')

def assert_sdmm(param):
    assert_tvprox(param)
    assert_positive_real_or_none(param, 'gamma')
    assert_positive_real(param, 'relative_radius')
    assert_positive_real_or_none(param, 'radius')
    assert_positive_integer(param.cg, 'max_iter')
    assert_positive_real(param.cg, 'tolerance')

def test_cg():
    from nose.tools import assert_equal, assert_almost_equal
    from purify.params import ConjugateGradient

    cg = ConjugateGradient(10, 1e-3)
    assert_equal(cg.max_iter, 10)
    assert_almost_equal(cg.tolerance, 1e-3)

    assert_positive_integer(cg, "max_iter")
    assert_positive_real(cg, "tolerance")

def test_sdmm():
    from nose.tools import assert_equal, assert_almost_equal
    from purify.params import SDMM

    param = SDMM( verbose='medium', max_iter=30, gamma=1e-2, relative_variation=1e-4,
                  radius=1e-2, relative_radius=1e-4, cg_max_iter=10, cg_tolerance=1e-4 )

    assert_equal(param.verbose, 'medium')
    assert_equal(param.max_iter, 30)
    assert_almost_equal(param.gamma, 1e-2)
    assert_almost_equal(param.relative_variation, 1e-4)
    assert_almost_equal(param.radius, 1e-2)
    assert_almost_equal(param.relative_radius, 1e-4)
    assert_equal(param.cg.max_iter, 10)
    assert_almost_equal(param.cg.tolerance, 1e-4)

    assert_sdmm(param)

def test_rw():
    from nose.tools import assert_equal, assert_almost_equal, assert_true
    from purify.params import RW

    param = RW( verbose='medium', max_iter=30, sigma=1e-2, relative_variation=1e-4,
                warm_start=False)

    assert_equal(param.verbose, 'medium')
    assert_equal(param.max_iter, 30)
    assert_almost_equal(param.sigma, 1e-2)
    assert_almost_equal(param.relative_variation, 1e-4)
    assert_true(not param.warm_start)

    assert_tvprox(param)
    assert_positive_real_or_none(param, 'sigma')
    assert_boolean(param, 'warm_start')


def test_tvprox():
    from nose.tools import assert_equal, assert_almost_equal
    from purify.params import TVProx

    param = TVProx(verbose='medium', max_iter=30, relative_variation=1e-4)
    assert_equal(param.verbose, 'medium')
    assert_equal(param.max_iter, 30)
    assert_almost_equal(param.relative_variation, 1e-4)

    assert_tvprox(param)

def test_tv_sdmm():
    from nose.tools import assert_equal, assert_almost_equal
    from purify.params import TV_SDMM

    param = TV_SDMM( verbose='medium', max_iter=30, gamma=1e-2, relative_variation=1e-4,
                     radius=1e-2, relative_radius=1e-4, cg_max_iter=10, cg_tolerance=1e-4,
                     tv_verbose='medium', tv_relative_variation=1e-4, tv_max_iter=30 )

    assert_equal(param.verbose, 'medium')
    assert_equal(param.max_iter, 30)
    assert_almost_equal(param.gamma, 1e-2)
    assert_almost_equal(param.relative_variation, 1e-4)
    assert_almost_equal(param.radius, 1e-2)
    assert_almost_equal(param.relative_radius, 1e-4)
    assert_equal(param.cg.max_iter, 10)
    assert_almost_equal(param.cg.tolerance, 1e-4)

    assert_equal(param.tv.verbose, 'medium')
    assert_equal(param.tv.max_iter, 30)
    assert_almost_equal(param.tv.relative_variation, 1e-4)

    assert_sdmm(param)
    assert_tvprox(param.tv)

def test_measurements():
    from nose.tools import assert_equal
    from purify.params import Measurements
    param = Measurements((2, 3), (4, 5), (6, 7))
    assert_equal(param.image_size, (2, 3))
    assert_equal(param.oversampling, (4, 5))
    assert_equal(param.interpolation, (6, 7))

    assert_size(param, 'image_size')
    assert_size(param, 'oversampling')
    assert_size(param, 'interpolation')


def test_apply_params():
    """ Test that apply params does apply parameters. """
    from nose.tools import assert_equal
    from purify.params import TVProx, apply_params
    class ApplyParamsChecker(TVProx):
        def __init__(self, **kwargs):
            super(ApplyParamsChecker, self).__init__(**kwargs)
            self.called = False
        @apply_params
        def __call__(self, value):
            assert_equal(self.verbose, 'medium')
            assert_equal(self.max_iter, 5)
            assert_equal(self.relative_variation, 1e-6)
            self.called = value
            return "My name is Inigo Montoya"
    # create a fake function object with some traits
    mock = ApplyParamsChecker(verbose='high', max_iter=6,
            relative_variation=1e-2)

    # Check the traits are what we expect them to be
    assert_equal(mock.verbose, 'high')
    assert_equal(mock.max_iter, 6)
    assert_equal(mock.relative_variation, 1e-2)
    assert_equal(mock.called, False)
    # Call the function object. It will check internally that it has been
    # modified.
    result = mock(True, verbose='medium', max_iter=5, relative_variation=1e-6)
    assert_equal(result, "My name is Inigo Montoya")
    # Check the traits have not changed when leaving the function
    assert_equal(mock.verbose, 'high')
    assert_equal(mock.max_iter, 6)
    assert_equal(mock.relative_variation, 1e-2)
    assert_equal(mock.called, True)

def test_apply_params_with_exception():
    """ Test that apply params unmodifies function when raising exception. """
    from nose.tools import assert_equal
    from purify.params import TVProx, apply_params
    class ApplyParamsChecker(TVProx):
        def __init__(self, **kwargs):
            super(ApplyParamsChecker, self).__init__(**kwargs)
            self.called = False
        @apply_params
        def __call__(self, value):
            assert_equal(self.verbose, 'medium')
            assert_equal(self.max_iter, 5)
            assert_equal(self.relative_variation, 1e-6)
            self.called = value
            raise RuntimeError("hello, you killed my father")
    # create a fake function object with some traits
    mock = ApplyParamsChecker(verbose='high', max_iter=6,
            relative_variation=1e-2)

    # Check the traits are what we expect them to be
    assert_equal(mock.verbose, 'high')
    assert_equal(mock.max_iter, 6)
    assert_equal(mock.relative_variation, 1e-2)
    assert_equal(mock.called, False)
    # Call the function object. It will check internally that it has been
    # modified.
    try: mock(True, verbose='medium', max_iter=5, relative_variation=1e-6)
    except RuntimeError as e:
      # Check the traits have not changed when leaving the function
      assert_equal(mock.verbose, 'high')
      assert_equal(mock.max_iter, 6)
      assert_equal(mock.relative_variation, 1e-2)
      assert_equal(mock.called, True)
      assert_equal(str(e), "hello, you killed my father")

    try: mock(False, verbose='medium', max_iter=-5, relative_variation=1e-6)
    except ValueError as e:
      # Check the traits have not changed when leaving the function
      assert_equal(mock.verbose, 'high')
      assert_equal(mock.max_iter, 6)
      assert_equal(mock.relative_variation, 1e-2)
      assert_equal(mock.called, True)
      assert_equal(str(e), "max_iter cannot be negative or null")


def test_pass_along():
    from nose.tools import assert_equal
    from purify.params import pass_along

    input = {'cg_verbose':0, 'cg_that': 'hello', 'verbose': 1, 'other': 2}
    actual = pass_along('cg', **input)
    expected = {'verbose': 0, 'that': 'hello'}
    assert_equal(actual, expected)
