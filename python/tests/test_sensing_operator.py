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

    shifts = exp(-1j * (
        visibility['u'] * dims[0] * 0.5 + visibility['v'] * dims[1] * 0.5
    ))
    assert_allclose(shifts, kernels.shifts)


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
    from numpy import mean, max, min
    from numpy.testing import assert_allclose

    _, image, sensing_op = visibility_image_operator()

    actual = sensing_op.forward(image.pixels)
    expected = [ -7.477447730559e-04 -1.643830300229e-04j,
         2.007158768045e-03 -4.642156936703e-03j,
        -2.174973032357e-02 +1.517973591831e-01j,
         1.163383706430e-03 +6.217717973737e-04j,
        -3.155582694953e-05 -1.757819582375e-04j,
        -5.556203800694e-03 +3.906914487911e-03j,
         4.446615519891e-04 +4.502868790349e-04j,
        -9.200048845186e-05 -5.950672269261e-05j,
         1.332859869049e-04 +3.411182895605e-03j,
        -3.042196522958e-02 +4.533805977039e-03j,
         4.747003134771e-04 -2.077543235753e-03j,
         8.678943808728e-04 -9.316061726497e-04j,
         1.703778883336e-03 +1.763662654315e-03j,
         2.588574384455e-03 -2.966714525229e-03j]
    assert_allclose(actual[::1000], expected)

    relative = lambda x, y: abs(x - y) / (abs(x) + abs(y))
    assert relative(mean(actual), 0.000219813613111-0.000147254689191j) < 1e-8
    assert relative(max(actual), 1.98141879458-0.136758307021j) < 1e-8
    assert relative(min(actual), -1.34576924606-0.683325812801j) < 1e-8


def test_harden_adjoint_sensing_operator():
    from numpy import mean, max, min
    from numpy.testing import assert_allclose

    _, image, sensing_op = visibility_image_operator()

    visibility = sensing_op.forward(image.pixels)
    actual = sensing_op.adjoint(visibility)

    expected = [[ 0.011831160109-0.001612501164j,  0.001946213142+0.0025705509j,
         0.003750765086-0.008416271113j, -0.003209420611+0.003529218959j],
       [ 0.001484072780+0.004132134996j, -0.012620479240+0.002641990815j,
        -0.007680678040-0.008379139764j,  0.005158227080-0.006272009878j],
       [-0.003627957258+0.001487793351j,  0.001330853181-0.002185753473j,
         0.008849886567-0.026886229575j,  0.002276177898-0.000421274356j],
       [ 0.000853000615+0.011684304937j, -0.003950378010+0.003324383869j,
         0.018507821221+0.020255682715j,  0.003660421284+0.001949924237j]]

    assert_allclose(actual[::64, ::64], expected, rtol=1e-5)

    relative = lambda x, y: abs(x - y) / (abs(x) + abs(y))
    assert relative(mean(actual), 0.000408592104098+0.00036371158722j) < 1e-8
    assert relative(max(actual), 0.0669724290646-0.0114304365868j) < 1e-8
    assert relative(min(actual), -0.0139962213705-0.000784060832947j) < 1e-8


def check_input_conversion(visibility, name, itis, expected, dtype):
    """ Checks the argument is converted as expected """
    from numpy import array, any, abs
    from numpy.testing import assert_allclose
    from copy import deepcopy
    from purify.sensing import visibility_column_as_numpy_array as vcana

    result = vcana(name, visibility)
    print(visibility)
    print(result)
    print(dtype)
    print(name)
    assert result.dtype == dtype
    assert_allclose(result, expected)

    copyme = deepcopy(expected)
    expected[:] = (1.0 + array(expected, dtype=dtype) ** 0.5)[:]
    if itis:
        assert_allclose(result, expected)
    else:
        assert any(abs(result - expected) > 1e-5)

    expected[:] = copyme


def test_input_conversion_dataframe():
    """ Input can be dataframe """
    from numpy import array
    from pandas import DataFrame

    vis = DataFrame({
        'u': [0, 1, 2, 3, 4],
        'v': [0.0, 1, 2, 3, 4],
        'y': [1j, 1., 2, 3, 4],
    })

    check_input_conversion(vis, 'u', False, vis['u'].values, 'double')
    check_input_conversion(vis, 'v', True, vis['v'].values, 'double')
    check_input_conversion(vis, 'y', True, vis['y'].values, 'complex')

    vis['y'] = array([1.0, 5.0, 6.0, 7, 78], dtype='double')
    check_input_conversion(vis, 'y', True, vis['y'].values, 'double')


def test_input_conversion_dictionary():
    """ Input can be dictionary """
    from numpy import array
    vis = {
        'u': [0, 1, 2, 3, 4],
        'v': [0.0, 1, 2, 3, 4],
        'y': [1j, 1., 2, 3, 4],
    }

    check_input_conversion(vis, 'u', False, vis['u'], 'double')
    check_input_conversion(vis, 'v', False, vis['v'], 'double')
    check_input_conversion(vis, 'y', False, vis['y'], 'complex')

    vis['u'] = array(vis['u'], dtype='double')
    vis['y'] = array([1.0, 5.0, 6.0, 7, 78], dtype='double')
    check_input_conversion(vis, 'u', True, vis['u'], 'double')
    check_input_conversion(vis, 'y', True, vis['y'], 'double')


def test_input_conversion_sequence():
    """ Input can be a sequence """
    from numpy import array
    vis = ([0, 1, 2, 3, 4], [0.0, 1, 2, 3, 4], [1j, 1., 2, 3, 4])

    check_input_conversion(vis, 'u', False, vis[0], 'double')
    check_input_conversion(vis, 'v', False, vis[1], 'double')
    check_input_conversion(vis, 'y', False, vis[2], 'complex')

    vis = (
        array(vis[0], dtype='double'),
        vis[1],
        array([1.0, 5.0, 6.0, 7, 78], dtype='double')
    )
    check_input_conversion(vis, 'u', True, vis[0], 'double')
    check_input_conversion(vis, 'y', True, vis[2], 'double')


def test_input_conversion_numpy_array():
    """ Input can be a homogeneous numpy array """
    from numpy import array
    vis = array([[0, 1, 2, 3, 4], [0.0, 1, 2, 3, 4], [1, 1., 2, 3, 4]])

    check_input_conversion(vis, 'u', True, vis[0], 'double')
    check_input_conversion(vis, 'v', True, vis[1], 'double')
    check_input_conversion(vis, 'y', True, vis[2], 'double')


def test_input_conversion_numpy_record():
    """ Input can be a numpy record array """
    from numpy import array
    vis = array(
        [(0, 0, 1j), (1.5, 1.6, 1.6), (5, 4, 3), (2, 1, 1.5+2j)],
        dtype=[('u', 'double'), ('v', 'double'), ('y', 'complex')]
    )

    check_input_conversion(vis, 'u', True, vis['u'], 'double')
    check_input_conversion(vis, 'v', True, vis['v'], 'double')
    check_input_conversion(vis, 'y', True, vis['y'], 'complex')
