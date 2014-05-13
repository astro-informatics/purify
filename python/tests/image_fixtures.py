""" Fixtures and tests to reproduce C examples """
from os.path import join, dirname
from purify import __file__ as datadir

datadir = join(dirname(datadir), "data")
visibility_files = {
    '30dor': join(datadir, "images", "Coverages", "cont_sim4.vis"),
}
""" Path to visibility (u, v) files """
image_files = {
    '30dor': join(datadir, "images", "M31.fits"),
}
""" Path oracle images from which dirty images are created """
expected_image_files = {
    '30dor': join(datadir, 'expected', "m31bpsa.fits"),
    '30dor_sara': join(datadir, 'expected', "m31sara.fits"),
    '30dor_tv': join(datadir, 'expected', "m31tv.fits"),
    '30dor_tvrw': join(datadir, 'expected', "m31rwtv.fits")
}
""" Path to expected outcome images """

def image_and_visibilities(name='30dor'):
    """ Reads input image and visibility from disk.

        Creates dirty visibility by adding noise to standard file.
    """
    from purify import read_visibility, read_image, MeasurementOperator as MO

    visibility = read_visibility(visibility_files[name])
    image = read_image(image_files[name])
    measurements = MO(visibility, image.shape, (2, 2), (24, 24))
    visibility['y0'] = measurements.forward(image)
    return image, visibility


def expected_images(name='30dor', method='sdmm'):
    """ Reads image results from disk. """
    from purify import read_image
    method = {'sdmm': '', 'sara': '_sara',
              'tv': '_tv', 'tvrw': '_tvrw'}[method]
    return read_image(expected_image_files[name + method])


def make_some_noise(visibility, n=None):
    """ Creates array of noise using sopt's very own numnber generator """
    from numpy.linalg import norm
    from numpy import sqrt, array
    from purify.tests.random import gaussian_distribution as gd

    sigma = norm(visibility['y0']) * 10.0**(-1.5) / sqrt(len(visibility))
    if n is None: n = len(visibility)
    return array([gd() + gd() * 1j for i in range(n)]) * sigma / sqrt(2)


def dirty_measurements(image, visibility):
    """ Creates fake dirty measurements. """
    return (visibility['y0'] + make_some_noise(visibility))

