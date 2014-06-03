""" Test internals of the main functor """


def test_deconvolution_kernel():
    """ Check C vs python deconvolution kernel """
    from os.path import join, dirname
    from numpy import add, exp
    from numpy.testing import assert_allclose
    from purify import kernels as create_kernels, read_visibility, \
                       __file__ as path

    path = join(dirname(path), "data", "images", "Coverages", "cont_sim4.vis")
    visibility = read_visibility(path)

    dims = 256, 256
    oversampling = 2, 2
    interpolation = 4, 4
    kernels = create_kernels(visibility, dims, oversampling, interpolation)

    def axis(image, oversampling, interpolation):
        from numpy import arange, pi
        scale = interpolation * pi / float(oversampling * image * image * 2)
        return (arange(0, image) - oversampling) * scale

    xaxis = axis(dims[0], oversampling[0], interpolation[0])
    yaxis = axis(dims[1], oversampling[1], interpolation[1])

    expected = exp(0.5 * add.outer(xaxis * xaxis, yaxis * yaxis))

    assert_allclose(expected, kernels.deconvolution)


def visibility_image_operator():
    """ Fixture: visibility, image, and sensing operator. """
    from os.path import join, dirname
    from purify import SensingOperator, read_visibility, Image, \
                       __file__ as path

    visibility_path = join(dirname(path), "data", "images",
                           "Coverages", "cont_sim4.vis")
    visibility = read_visibility(visibility_path)

    image_path = join(dirname(path), "data", "images", "M31.fits")
    image = Image(image_path)

    def nearest_power_of_two(x):
        from numpy import log, round
        return 2**int(0.1 + round(log(x) / log(2)))
    dims = nearest_power_of_two(image.shape[0]), \
           nearest_power_of_two(image.shape[1])
    oversampling = 2, 2
    interpol = 4, 4
    sensing_op = SensingOperator(visibility, dims, oversampling, interpol)

    return visibility, image, sensing_op


def test_harden_forward_sensing_operator():
    from nose.tools import assert_almost_equal
    from numpy import mean, max, min
    from numpy.testing import assert_allclose

    _, image, sensing_op = visibility_image_operator()

    actual = sensing_op.forward(image.pixels)
    expected = [2.03724422e-04 + 4.15991536e-04j,
                1.08735157e-03 - 6.79113430e-03j,
               -1.60430819e-02 + 1.60710447e-01j,
               -7.38552963e-04 - 1.19728759e-03j,
                2.97760267e-05 - 1.82804158e-04j,
                7.26545343e-03 - 5.90595568e-03j,
               -6.70682680e-04 - 5.40322258e-04j,
                1.02806690e-04 + 2.30846267e-05j,
                1.33592565e-03 + 3.61525331e-03j,
                2.80500284e-02 - 2.41679083e-02j,
               -5.99541058e-04 + 2.08489869e-03j,
               -5.95013573e-04 + 1.53026842e-03j,
               -2.22493322e-03 - 1.41642788e-03j,
                1.81243121e-03 - 4.02146676e-03j]
    assert_allclose(actual[::1000], expected)

    assert_almost_equal(mean(actual), 0.000787855678485+0.000115664948325j)
    assert_almost_equal(max(actual), 2.01111312538-0.57210242831j)
    assert_almost_equal(min(actual), -0.956794022954+0.893428684183j)


def test_harden_adjoint_sensing_operator():
    from nose.tools import assert_almost_equal
    from numpy import mean, max, min
    from numpy.testing import assert_allclose

    _, image, sensing_op = visibility_image_operator()

    visibility = sensing_op.forward(image.pixels)
    actual = sensing_op.adjoint(visibility)

    expected = [[ 0.01558801 + 0.00076014j, -0.00089624 + 0.00424112j,
                  0.00616964 - 0.01035894j, -0.00577823 + 0.00277242j],
                [ 0.00184355 + 0.00612539j, -0.01514326 + 0.00366365j,
                 -0.00876138 - 0.00948248j,  0.00554129 - 0.00681591j],
                [-0.00583695 + 0.00231628j,  0.00205820 - 0.00213758j,
                  0.01177767 - 0.03686787j,  0.00258941 - 0.00038525j],
                [-0.00167731 + 0.01125487j, -0.00452309 + 0.00287998j,
                  0.02097403 + 0.02555827j,  0.00458462 + 0.00225583j]]

    assert_allclose(actual[::64, ::64], expected, rtol=1e-5)

    assert_almost_equal(mean(actual), 0.000481266445518+0.000282571620767j)
    assert_almost_equal(max(actual), 0.0911069005447-0.0129906262887j)
    assert_almost_equal(min(actual), -0.0168798725996-0.000672209789272j)
