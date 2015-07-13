""" Tests SDMM by reproducing example_30dor.c """


def test_l1_sdmm():
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


def test_l1_rw_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, \
        expected_images, dirty_measurements

    image, visibility = image_and_visibilities()
    expected = expected_images('30dor', 'sara')

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    rwsdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets,
                  reweighted=True)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    rwsdmm.rw.sigma = sigma * sqrt(len(visibility)) \
        / sqrt(len(rwsdmm) * image.size)
    rwsdmm.rw.max_iter = 3
    rwsdmm.relative_variation = 1e-3
    rwsdmm.rw.relative_variation = 1e-3
    actual = rwsdmm(visibility, radius=radius, max_iter=3)

    assert_allclose(actual.real, expected)


def test_tv_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, \
        expected_images, dirty_measurements

    image, visibility = image_and_visibilities()
    expected = expected_images('30dor', 'tv')

    reset()
    sdmm = SDMM(image_size=image.shape)

    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    actual = sdmm(visibility, radius=radius, max_iter=6, tv_norm=True,
                  relative_variation=1e-3)

    assert_allclose(actual.real, expected)


def test_tv_rw_sdmm():
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import SDMM
    from purify.tests.image_fixtures import image_and_visibilities, \
        expected_images, dirty_measurements

    image, visibility = image_and_visibilities()
    expected = expected_images('30dor', 'tvrw')

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    rwsdmm = SDMM(image_size=image.shape, nblevels=4, wavelets=wavelets,
                  reweighted=True, tv_norm=True)
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(1e0 + 2.0 / sqrt(len(visibility))) * sigma               \
        * sqrt(image.size)
    rwsdmm.rw.sigma = sigma * sqrt(len(visibility)) \
        / sqrt(len(rwsdmm) * image.size)
    rwsdmm.rw.max_iter = 2
    rwsdmm.relative_variation = 1e-3
    rwsdmm.rw.relative_variation = 1e-3
    rwsdmm.rw.warm_start = False
    actual = rwsdmm(visibility, radius=radius, max_iter=3)

    assert_allclose(actual.real, expected)
