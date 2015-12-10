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
        (-0.00234567658459+0.00204441401177j),
        (0.00462507471963+2.17722861998e-05j),
        (-0.00136630046842-0.00158291050687j),
         (-0.0090330014461+0.00596977619358j),
          (0.0130421262009+0.00417077187357j),
        (0.00135540334366+0.000369626418828j),
         (0.00604695790661-0.00831693798277j),
      (-0.000245102108841+0.000671438634195j),
          (0.0161011516402-0.00300725287261j),
      (-0.000273210042299+0.000129677853961j)
    ]
    scale = sqrt(image.size) / sqrt(len(visibility))
    assert_allclose(actual, expected / scale)
