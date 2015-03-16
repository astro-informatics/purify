""" Purify package """
__docformat__ = "restructuredtext en"
__all__ = ['read_visibility', 'Image', 'kernels', 'SensingOperator',
           'SparsityOperator', 'SDMM', 'read_image', 'RWSDMM']
from os.path import exists, join, dirname

from .visibility import read_visibility
from .image import Image
from .sensing import kernels, SensingOperator
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


def test():
    from py.test import main
    from os.path import dirname
    main(dirname(__file__))


# Register casa task on import if:
#     - this is an IPython session
#     - this is a casapy session
#     - the task has been built
try:
    from __builtin__ import __IPYTHON__
    if hasattr(__IPYTHON__, 'getapi'):
        api = __IPYTHON__.getapi()
        task_file = join(dirname(__file__), 'casa', 'task.py')
        if 'casalog' in api.user_ns and exists(task_file):
            execfile(task_file, api.user_ns)

        from .casa import data_iterator, purified_iterator
        __all__.extend(['data_iterator', 'purified_iterator'])
        # Clean-up purify namespace
        del api
        del task_file
    del __IPYTHON__
except ImportError:
    pass
