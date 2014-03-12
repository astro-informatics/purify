""" Test internals of the main functor """

def test_deconvolution_kernel():
    """ Check C vs python deconvolution kernel """
    from os.path import join, dirname
    from numpy import add, exp
    from numpy.testing import assert_allclose
    from purify import kernels as create_kernels, read_visibility, __file__ as path

    path = join(dirname(path), "data", "images", "Coverages", "cont_sim4.vis")
    visibility = read_visibility(path)

    image_size = 256, 256
    oversampling = 2, 2
    interpolation_size = 4, 4
    kernels = create_kernels(visibility, image_size, oversampling, interpolation_size)

    def axis(image, oversampling, interpolation):
        from numpy import arange, pi
        scale = interpolation * pi / float(oversampling * image * image * 2) 
        return (arange(0, image) - oversampling) * scale

    xaxis = axis(image_size[0], oversampling[0], interpolation_size[0])
    yaxis = axis(image_size[1], oversampling[1], interpolation_size[1])
    
    expected = exp(0.5 * add.outer(xaxis * xaxis, yaxis * yaxis))

    assert_allclose(expected, kernels.deconvolution)
