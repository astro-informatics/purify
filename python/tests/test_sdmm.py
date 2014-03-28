""" Tests SDMM by reproducing example_30dor.c """
def test_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, \
                               expected_images, dirty_measurements
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
