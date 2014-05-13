""" Purify package """
__docformat__ = "restructuredtext en"
__all__ = ['read_visibility', 'Image', 'kernels', 'MeasurementOperator',
           'SparsityOperator', 'SDMM', 'read_image', 'RWSDMM']
from .visibility import read_visibility
from .image import Image
from .measurements import kernels, MeasurementOperator
from .sparsity_ops import SparsityOperator
from .sdmm import SDMM


def read_image(path, power_of_two=True):
    """ Reads an image from disk

        Parameters:
            path: str
                Path to image file
            power_of_two: boolean
                If true, then image size is a power of two. Actual image is in
                top left corner (0:n, 0:m range)
    """
    from numpy import zeros, log, array, ceil
    image = Image(path).pixels
    if power_of_two:
        shape = array(image.shape).astype('double')
        shape = 2**ceil(log(shape) / log(2.0))
        shape = tuple(shape)
        topleft = zeros(shape, dtype=image.dtype, order='C')
        topleft[:image.shape[0], :image.shape[1]] = image
        image = topleft
    return topleft
