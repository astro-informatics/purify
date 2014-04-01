""" Tests SDMM by reproducing example_30dor.c """
from os.path import join, dirname
from purify import __file__ as datadir

datadir = join(dirname(datadir), "data")
visibility_files = {
    '30dor': join(datadir, "images", "Coverages", "cont_sim4.vis"),
}
""" Path to visibility (u, v) files """
image_files = {
    '30dor': join(datadir, "images", "M31.fits"),
}
""" Path oracle images from which dirty images are created """
expected_image_files = {
    '30dor': join(datadir, 'expected', "m31bpsa.fits"),
}
""" Path to expected outcome images """

def image_and_visibilities(name='30dor'):
    """ Reads input image and visibility from disk.

        Creates dirty visibility by adding noise to standard file.
    """
    from purify import read_visibility, read_image, MeasurementOperator as MO

    visibility = read_visibility(visibility_files[name])
    image = read_image(image_files[name])
    measurements = MO(visibility, image.shape, (2, 2), (24, 24))
    visibility['y0'] = measurements.forward(image)
    return image, visibility


def expected_images(name='30dor'):
    """ Reads image results from disk. """
    from purify import read_image
    return read_image(expected_image_files[name])



def make_some_noise(visibility, n=None):
    """ Creates array of noise using sopt's very own numnber generator """
    from numpy.linalg import norm
    from numpy import sqrt, array
    from purify.tests.random import gaussian_distribution as gd

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    if n is None: n = len(visibility)
    return array([gd() + gd() * 1j for i in range(n)]) * sigma / sqrt(2)


def dirty_measurements(image, visibility):
    """ Creates fake dirty measurements. """
    return (visibility['y0'] + make_some_noise(visibility))


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


def test_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import SDMM
    image, visibility = image_and_visibilities()
    expected = expected_images('30dor')

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    sdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
              * sqrt(image.size)
    actual = sdmm(visibility, radius=radius, max_iter=5)

    assert_allclose(actual.real, expected)
