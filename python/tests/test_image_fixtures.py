""" Tests fixture give the right results """
from purify.tests.image_fixtures import image_and_visibilities, \
                            make_some_noise, dirty_measurements

def test_harden_random():
    """ Checks that random numbers are fixed by seed

        These numbers are used to generate fake dirty images from cleaned
        images in both C and python code. They should remain the same.
    """
    from numpy.testing import assert_allclose
    from purify.tests.random import reset

    image, visibility = image_and_visibilities()

    reset()
    actual = make_some_noise(visibility, 5)
    expected = [
        -0.0004048868413905625 + 0.0004998856543791353j,
         0.0010252113233472518 + 0.0006731309681747155j,
        -0.0006402841370415399 - 0.000710428273313625j,
        -0.0015624288645055503 - 0.0016640991757716221j,
         0.0015712341522236288 + 0.0008254357025121923j
    ]
    assert_allclose(actual, expected)


def test_harden_reset():
    """ Make sure we can reset the random number generator. """
    test_harden_random()
    test_harden_random()


def test_harden_dirty_image():
    """ Make sure dirty image stays what it is. """
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    image, visibility = image_and_visibilities()

    reset()
    actual = dirty_measurements(image, visibility)[:10]
    expected = [
         (-0.0025779629409+0.000750385437798j),
          (0.00114034979371-0.00100676562853j),
         (-0.00136686865551-0.00159946686582j),
         (-0.00198922875103+0.00733872190704j),
         (-0.0060053359212-0.000512419375827j),
          (0.00266958514619-0.00375981242741j),
          (0.00690335501097-0.00766305939611j),
        (-0.000400339081838+0.00084899489691j),
            (0.0034246920022-0.0124356716948j),
          (0.000650022911397+0.0005080970237j),
    ]
    scale = sqrt(image.size) / sqrt(len(visibility))
    assert_allclose(actual, expected / scale)
