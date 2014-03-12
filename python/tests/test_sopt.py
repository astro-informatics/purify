""" Test SOPT bindings """

def test_sara():
    """ Create basis-functions object """
    from nose.tools import assert_equal
    from purify.sopt import BasisFunctions

    shape = 256, 256
    nlevels = 5
    types = ["DB1", "DB2", "DB10", "Dirac"]
    functions = BasisFunctions(shape, nlevels, types)

    assert_equal(len(functions), len(types))
    assert_equal(set([u.lower() for u in functions]), set([u.lower() for u in types]))
  
    assert_equal(functions.nlevels, nlevels)
    assert_equal(functions.shape, shape)
