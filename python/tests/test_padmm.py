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
    from numpy import abs, max, nonzero
    from numpy.linalg import norm
    from numpy.testing import assert_allclose
    from numpy import sqrt
    from purify.tests.random import reset
    from purify import PADMM
    from purify.tests.image_fixtures import image_and_visibilities, read_dirty_visibility

    image, visibility = image_and_visibilities()

    reset()
    wavelets = ['DB%i' % i for i in range(1, 9)] + ['Dirac']
    padmm = PADMM(
        image_size=image.shape, nblevels=4, wavelets=wavelets,
        oversampling=(2, 2), interpolation=(24, 24),
        verbose=2, max_iter=1, gamma=0.01, relative_variation=1e-4,
        epsilon_tol_scale=1.001, lagrange_update_scale=0.9,
        l1={
            'verbose': 0, 'max_iter': 20, 'relative_variation': 1e-2,
            'nu': 1e0, 'tight_frame': False, 'positivity': True
        }
    )
    visibility['y'] = read_dirty_visibility("m31")

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    radius = sqrt(len(visibility) + 2.0 * sqrt(len(visibility))) * sigma
    actual = padmm(visibility, radius=radius, max_iter=5, scale=None)

    expected = [
          0.0401400309968165, 0.0340251725201707, 0.0416609314108698,
          0.0104533234341291, 0.0252994121980796, 0.2838342505345737,
          0.0206193554927490, 0.0787079263167902, 0.0130456464057992,
          0.0043258526182543, 0.0467178535165592, 0.0242556046828754,
          0.0242608104434959, 0.1185918936128116, 0.0764981742687284,
          0.0336467967238792, 0.0020411233245216, 0.0365907878118061,
          0.0375271694281585, 0.0813220282293167, 0.0699034803634203,
          0.0168826565221940, 0.0499503040150118, 0.0890886204099229,
          0.0149429990682312, 0.0904031073993032, 0.0797271791061975,
          0.0176067786322632, 0.1428506042977737, 0.1243447691164086,
          0.1100769900348644, 0.0429891230476658, 0.1215684158614487,
          0.0505009039249495, 0.0651218506543877, 0.0111822453334854,
          0.0402443774544712, 0.0522886913195477, 0.0090582828415466,
          0.0377735720980643, 0.0577017340732379, 0.0022591820139794
    ]
    assert_allclose(actual.flat[nonzero(actual.flat)][::1000], expected, rtol=1e-4, atol=1e-5)
