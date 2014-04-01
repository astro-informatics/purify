""" Tests python image bindings """
def test_Image_module():
    """ Check Image module is purify.image. """
    from nose.tools import assert_equal
    from purify import Image
    assert_equal(Image.__module__, 'purify.image')

def test_read_image():
    from nose.tools import assert_equal, assert_almost_equal
    from os.path import join, dirname
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "M31.fits")
    image = Image(filename=location)

    assert_equal(image.dtype, 'double')
    assert_equal(image.pixels.dtype, 'double')
    assert_equal(image.ndim, 2)
    assert_equal(image.pixels.ndim, 2)
    assert_equal(image.shape, (256, 256))
    assert_equal(image.pixels.shape, (256, 256))
    assert_almost_equal(image.fov[0], 0)
    assert_almost_equal(image.fov[1], 0)

def test_set_fov():
    from nose.tools import assert_almost_equal
    from purify import Image

    image = Image(fov=(15, 16))
    assert_almost_equal(image.fov[0], 15)
    assert_almost_equal(image.fov[1], 16)

    image.fov = (18, 19)
    assert_almost_equal(image.fov[0], 18)
    assert_almost_equal(image.fov[1], 19)


def test_read_write_cycle():
    """ Reads, then writes, the re-reads the same image """
    from nose.tools import assert_equal, assert_almost_equal
    from numpy.testing import assert_allclose
    from os.path import join, dirname
    from tempfile import NamedTemporaryFile
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "30DOR.fits")
    first = Image(filename=location)

    assert_equal(first.shape, (251, 251))
    assert_almost_equal(first.fov[0], 0)
    assert_almost_equal(first.fov[1], 0)
    assert_equal(first.ndim, 2)

    with NamedTemporaryFile(delete=True) as file:
        file.close()
        first.write(file.name)

        second = Image(filename=location, fov=(16, 17))
        assert_equal(second.shape, (251, 251))
        assert_almost_equal(second.fov[0], 16)
        assert_almost_equal(second.fov[1], 17)
        assert_equal(second.ndim, 2)
        assert_allclose(second.pixels, first.pixels)

def test_change_pixels():
    """ Pixels should always be double and 2d. """
    from nose.tools import assert_equal, assert_raises
    from numpy.testing import assert_allclose
    from os.path import join, dirname
    from purify import Image, __file__ as location
    location = join(dirname(location), "data", "images", "30DOR.fits")
    image = Image(filename=location)

    # This should resize image automatically, as well as cast input to double
    image.pixels = [[1, 2], [2, 3]]
    assert_allclose(image.pixels, [[1, 2], [2, 3]])
    assert_equal(image.shape, (2, 2))
    assert_equal(image.dtype, 'double')


    # This should fail since pixels are always double
    with assert_raises(ValueError) as exception:
      image.pixels = [['a', 'a'], ['a', 'a']]
