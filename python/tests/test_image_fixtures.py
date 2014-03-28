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
    expected = [ -4.580937149521e-04 + 5.655764847267e-04j,
                  1.159936099949e-03 + 7.615882620473e-04j,
                 -7.244249725550e-04 - 8.037868699598e-04j,
                 -1.767750318661e-03 - 1.882781299732e-03j,
                  1.777712724325e-03 + 9.339076224834e-04j ]
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
    expected = [ -5.689230522507e-04 + 2.195359868245e-03j,
                  3.695128919703e-03 + 4.310356844945e-03j,
                 -1.212943123351e-03 - 1.727162167302e-03j,
                  1.218825657186e-02 + 2.547311994654e-03j,
                 -6.115781968455e-03 - 1.284011163531e-04j,
                  3.797738450268e-03 - 3.719263640609e-03j,
                  3.085729358754e-02 - 2.765698977712e-02j,
                 -3.711241133917e-04 + 1.235131172866e-03j,
                  1.503499807558e-02 - 1.260196332277e-02j,
                 -3.518734857389e-04 - 1.600332282335e-04j ]
    scale = sqrt(image.size) / sqrt(len(visibility))
    assert_allclose(actual, expected / scale)
