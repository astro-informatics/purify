""" Functionality for interfacing CASA and Purify via pyrap """
__docformat__ = 'restructuredtext en'
__all__ = ['data_iterator', 'purified_iterator']
from . import casa

class DataTransform(casa.CasaTransform):
    def _get_table(self, name=None):
        """ A table object """
        # can't use standard import since it would identify pyrap as this very
        # module, rather than the global pyrap module.
        tables = __import__('pyrap.tables', globals(), locals(), ['table'], 0)
        if name is None:
            return tables.table(self.measurement_set)
        return tables.table("%s::%s" % (self.measurement_set, name))
    def _c_vs_fortran(self, value):
        """ Function to derive in pyrap wrappers """
        return value.T

# Wraps around generators in purify.casa and makes them use the DataTransform
# employed above, rather than the purify.casa one.
def purify_image(datatransform, imagename, imsize=(128, 128), overwrite=False,
        coordsys=None, **kwargs):
    """ Creates an image from a given measurement set

        Parameters:
            ms: str
                Path to the measurement set
            imagename: str
                Path to the output CASA image
            overwrite: bool
                Whether to overwrite `imagename` if it already exists. Defaults
                to False.
            *args, **kwargs:
                See :py:class:`purify.casa.CasaTransform` and
                :py:func:`purify.casa.purify_measurement_set`
    """
    from numpy import real, array
    from os.path import abspath
    from . import SDMM
    # can't use standard import since it would identify pyrap as this very
    # module, rather than the global pyrap module.
    images = __import__('pyrap.images', globals(), locals(), ['image'], 0)
    stokes = __import__(
            'pyrap.images.coordinates', globals(), locals(),
            ['stokescoordinate'], 0
    )

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)

    scale = kwargs.pop('scale', 'default')
    sdmm = SDMM(image_size=imsize, **kwargs)
    data = array(
        [real(sdmm(x, scale=scale)) for x in datatransform],
        dtype = 'double'
    )
    imagename = abspath(imagename)
    # argument "value = something" seems to be broken
    image = images.image(
        abspath(imagename),
        shape=data.shape,
        overwrite=overwrite,
        coordsys=coordsys
    )
    if coordsys is None:
        coordsys = stokes.stokescoordinate("I")
    image.put(data)
    return image

# Wraps around generators in purify.casa and makes them use the DataTransform
# employed above, rather than the purify.casa one.
def purify_measurement_set(ms, imagename, imsize=(128, 128), datadescid=0,
        ignoreW=False, channels=None, column=None, resolution=0.3, **kwargs):
    """ Creates an image from a given measurement set

        Parameters:
            ms: str
                Path to the measurement set
            imagename: str
                Path to the output CASA image
            overwrite: bool
                Whether to overwrite `imagename` if it already exists. Defaults
                to False.
            *args, **kwargs:
                See :py:class:`purify.casa.CasaTransform` and
                :py:func:`purify.casa.purify_measurement_set`
    """
    transform = DataTransform(
        ms, channels=channels, datadescid=datadescid, column=column,
        ignoreW=ignoreW, resolution=resolution
    )

    return purify_image(transform, imagename=imagename, imsize=imsize,
            **kwargs)
