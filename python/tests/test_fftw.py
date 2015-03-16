def test_data_bindings():
    from purify.fftw import Fourier2D
    from numpy.testing import assert_allclose

    image_size = 256, 256
    oversampling = 2, 2
    shape = image_size[0] * oversampling[0], image_size[1] * oversampling[1]

    fourier = Fourier2D(image_size, oversampling, "forward")

    # Changing the data twice makes sure we are binding to memory rather than
    # python object
    fourier.data[:] = 0
    assert fourier.data.shape == shape
    assert_allclose(fourier.data, 0)

    fourier.data[:] = 1
    assert_allclose(fourier.data, 1)


def test_cycle_fftw():
    """ Apply forward and backward fft """
    from purify.fftw import Fourier2D
    from numpy import add, exp, arange, product
    from numpy.testing import assert_allclose

    image_size = 256, 256
    oversampling = 2, 2
    shape = image_size[0] * oversampling[0], image_size[1] * oversampling[1]

    forward = Fourier2D(image_size, oversampling, "forward")
    backward = Fourier2D(image_size, oversampling, "backward")

    sigma = 10.0, 8.0
    xaxis = (arange(shape[0]) / float(shape[0]) - 0.5) / sigma[0]
    yaxis = (arange(shape[1]) / float(shape[1]) - 0.5) / sigma[1]

    expected = exp(-add.outer(xaxis * xaxis, yaxis * yaxis))
    forward.data = expected
    forward.execute()
    backward.data = forward.data
    backward.execute()

    assert_allclose(backward.data, expected * product(forward.data.shape))
