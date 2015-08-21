""" Tests PADMM by reproducing example_m31_padmm.c """


def test_params():
    # from numpy.testing import assert_allclose
    from numpy import abs
    from purify import PADMM

    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    padmm = PADMM(
        image_size=(256, 256), nblevels=4, wavelets=wavelets,
        verbose=2, max_iter=200, gamma=0.01, relative_variation=1e-4,
        epsilon_tol_scale=1.001, lagrange_update_scale=0.9,
        l1={
            'verbose': 0, 'max_iter': 20, 'relative_variation': 1e-2,
            'nu': 1e0, 'tight_frame': False, 'positivity': True
        }
    )

    assert padmm.verbose == "high"
    assert padmm.max_iter == 200
    assert abs(padmm.gamma - 1e-2) < 1e-8
    assert abs(padmm.relative_variation - 1e-4) < 1e-10
    assert abs(padmm.epsilon_tol_scale - 1.001) < 1e-8
    assert abs(padmm.lagrange_update_scale - 0.9) < 1e-8
    assert padmm.l1.verbose == 'off'
    assert padmm.l1.max_iter == 20
    assert abs(padmm.l1.relative_variation - 1e-2) < 1e-8
    assert abs(padmm.l1.nu - 1e0) < 1e-8
    assert not padmm.l1.tight_frame
    assert padmm.l1.positivity


def test_padmm():
    from numpy.linalg import norm
    # from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import PADMM
    from purify.tests.image_fixtures import image_and_visibilities, \
        dirty_measurements
    # , expected_images

    image, visibility = image_and_visibilities()
    # expected = expected_images('M31', method='padmm')

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    padmm = PADMM(
        image_size=image.shape, nblevels=4, wavelets=wavelets,
        oversampling=(2, 2), interpolation=(24, 24),
        verbose=2, max_iter=200, gamma=0.01, relative_variation=1e-4,
        epsilon_tol_scale=1.001, lagrange_update_scale=0.9,
        l1={
            'verbose': 0, 'max_iter': 20, 'relative_variation': 1e-2,
            'nu': 1e0, 'tight_frame': False, 'positivity': True
        }
    )
    visibility['y'] = dirty_measurements(image, visibility)

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(len(visibility) + 2.0 * sqrt(len(visibility))) * sigma
    padmm(visibility, radius=radius, max_iter=5, scale=None)
    # actual = padmm(visibility, radius=radius, max_iter=5)

    # assert_allclose(actual.real, expected)

test_padmm()
