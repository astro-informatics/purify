""" Tests python image bindings """


def test_Image_module():
    """ Check Image module is purify.image. """
    from purify import Image
    assert Image.__module__ == 'purify.image'


def test_read_image():
    from os.path import join, dirname
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "M31.fits")
    image = Image(filename=location)

    assert image.dtype == 'double'
    assert image.pixels.dtype == 'double'
    assert image.ndim == 2
    assert image.pixels.ndim == 2
    assert image.shape == (256, 256)
    assert image.pixels.shape == (256, 256)
    assert abs(image.fov[0]) < 1e-8
    assert abs(image.fov[1]) < 1e-8


def test_set_fov():
    from purify import Image

    image = Image(fov=(15, 16))
    assert abs(image.fov[0] - 15) < 1e-8
    assert abs(image.fov[1] - 16) < 1e-8

    image.fov = (18, 19)
    assert abs(image.fov[0] - 18) < 1e-8
    assert abs(image.fov[1] - 19) < 1e-8


def test_read_write_cycle():
    """ Reads, then writes, the re-reads the same image """
    from numpy.testing import assert_allclose
    from os.path import join, dirname
    from tempfile import NamedTemporaryFile
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "30dor.fits")
    first = Image(filename=location)

    assert first.shape == (251, 251)
    assert abs(first.fov[0]) < 1e-8
    assert abs(first.fov[1]) < 1e-8
    assert first.ndim == 2

    with NamedTemporaryFile(delete=True) as file:
        file.close()
        first.write(file.name)

        second = Image(filename=location, fov=(16, 17))
        assert second.shape == (251, 251)
        assert abs(second.fov[0] - 16) < 1e-8
        assert abs(second.fov[1] - 17) < 1e-8
        assert second.ndim == 2
        assert_allclose(second.pixels, first.pixels)


def test_change_pixels():
    """ Pixels should always be double and 2d. """
    from py.test import raises
    from numpy.testing import assert_allclose
    from os.path import join, dirname
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "30dor.fits")
    image = Image(filename=location)

    # This should resize image automatically, as well as cast input to double
    image.pixels = [[1, 2], [2, 3]]
    assert_allclose(image.pixels, [[1, 2], [2, 3]])
    assert image.shape == (2, 2)
    assert image.dtype == 'double'

    # This should fail since pixels are always double
    with raises(ValueError):
        image.pixels = [['a', 'a'], ['a', 'a']]
