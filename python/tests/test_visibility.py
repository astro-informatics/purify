""" Tests visibility bindings """

def test_read_visibility():
    # A fake visibility is first created and written to file
    # Then the file is read and the two visibilities (fake and reread) are compared. 
    from pandas  import DataFrame
    from numpy import sqrt
    from numpy.random import random
    from numpy.testing import assert_allclose, assert_equal
    from tempfile import NamedTemporaryFile
    from purify import read_visibility

    N = 10
    noise = random(N)
    expected = DataFrame({
        'u': random(N), 'v': random(N), 'w': [0] * N,
        'noise': (1+1j) / sqrt(2) * noise, 'y': random(N) + 1j * random(N)
    })

    csv = DataFrame({
        'u': expected['u'], 'v': expected['v'],
        'yreal': expected['y'].real, 'yimag': expected['y'].imag,
        'noise': noise
    })

    with NamedTemporaryFile(delete=True) as file:
        file.close()
        csv.to_csv(file.name, header=False, cols=['u', 'v', 'yreal', 'yimag', 'noise'])
        actual = read_visibility(file.name)

        assert_equal(set(expected.keys()), set(expected.keys()))
        for name in expected.keys(): 
            assert_allclose( actual[name], expected[name], 
                             err_msg = "Columns %s did not compare" % name )
