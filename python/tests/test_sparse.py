""" Test sparse matrix C/python conversion and wrapping """
def test_python_to_c():
    """ Wrapping a C-sparse matrix structure around a known scipy sparse matrix """
    from scipy.sparse import csr_matrix
    from numpy import identity, zeros
    from numpy.testing import assert_allclose
    from purify.griding import _debug_multiply

    matrix = identity(4, dtype="double")
    matrix[1, 1] = 2
    matrix = csr_matrix(matrix)

    for i in xrange(matrix.shape[1]):
      x = zeros(matrix.shape[1], dtype="double") + 1
      x[2] = 0

      actual = _debug_multiply(matrix, x)
      expected = matrix * x
      
      assert_allclose(actual, expected)


def test_python_to_c_random():
    """ Wrapping a C-sparse matrix structures around random scipy sparse matrices """
    from scipy.sparse import rand
    from numpy.random import randint, random_sample
    from numpy.testing import assert_allclose
    from numpy import dtype
    from purify.griding import _debug_multiply

    for dtype in [dtype('double'), dtype('complex')]:
        for i in range(10):
            shape = randint(3, 90), randint(3, 90)
            matrix = rand(*shape, density=0.4, format = 'csr', dtype=dtype)
            x = random_sample(shape[1])
            if dtype == "complex": x = x + 1j * random_sample(shape[1])

            actual = _debug_multiply(matrix, x)
            expected = matrix * x
            assert_allclose(actual, expected)

def test_cycle_python_to_c_to_python():
    """ Wraps a C object around python sparse matrix, and converts back. """
    from scipy.sparse import rand
    from numpy.random import randint
    from numpy.testing import assert_allclose
    from numpy import dtype
    from nose.tools import assert_equal
    from purify.griding import _cycle_python_to_c_to_python

    for dtype in [dtype('double'), dtype('complex')]:
        for i in range(10):
            shape = randint(3, 90), randint(3, 90)
            expected = rand(*shape, density=0.4, format = 'csr', dtype=dtype)

            actual = _cycle_python_to_c_to_python(expected)

            assert_equal(actual.shape, expected.shape)
            assert_equal(actual.nnz, expected.nnz)
            assert_allclose(actual.data, expected.data)
            assert_allclose(actual.indices, expected.indices)
            assert_allclose(actual.indptr, expected.indptr)
