""" Test sparse matrix C/python conversion and wrapping """


def test_python_to_c():
    """ Wrapping C-sparse matrix structure around known scipy sparse matrix """
    from scipy.sparse import csr_matrix
    from numpy import identity, zeros
    from numpy.testing import assert_allclose
    from purify.tests.sparse_testing import _debug_multiply

    matrix = identity(4, dtype="double")
    matrix[1, 1] = 2
    matrix = csr_matrix(matrix)

    for i in xrange(matrix.shape[1]):
        x = zeros(matrix.shape[1], dtype="double", order="C") + 1
        x[2] = 0

        actual = _debug_multiply(matrix, x)
        expected = matrix * x
        assert_allclose(actual, expected)


def test_python_to_c_random():
    """ Wrapping C-sparse matrix with random scipy sparse matrices """
    from scipy.sparse import rand
    from numpy.random import randint, random_sample
    from numpy.testing import assert_allclose
    from numpy import dtype
    from purify.tests.sparse_testing import _debug_multiply

    for type in [dtype('complex')]:
        for i in range(1):
            shape = randint(3, 90), randint(3, 90)
            matrix = rand(*shape, density=0.4, format='csr', dtype=type)
            x = random_sample(shape[1])
            if type == "complex":
                x = x + 1j * random_sample(shape[1])

            actual = _debug_multiply(matrix, x)
            expected = matrix * x
            assert_allclose(actual, expected, atol=1e-7)


def test_cycle_python_to_c_to_python():
    """ Wraps a C object around python sparse matrix, and converts back. """
    from scipy.sparse import rand
    from numpy.random import randint
    from numpy.testing import assert_allclose
    from numpy import dtype
    from purify.tests.sparse_testing import _cycle_python_to_c_to_python

    for type in [dtype('double'), dtype('complex')]:
        for i in range(10):
            shape = randint(3, 90), randint(3, 90)
            expected = rand(*shape, density=0.4, format='csr', dtype=type)

            actual = _cycle_python_to_c_to_python(expected)

            assert actual.shape == expected.shape
            assert actual.nnz == expected.nnz
            assert_allclose(actual.data, expected.data)
            actual.data[0] += 2  # both arrays refer to same underlying data
            assert_allclose(actual.data, expected.data)
            assert_allclose(actual.indices, expected.indices)
            assert_allclose(actual.indptr, expected.indptr)
