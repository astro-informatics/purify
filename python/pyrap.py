""" Functionality for interfacing CASA and Purify via pyrap """
from . import casa

__docformat__ = 'restructuredtext en'
__all__ = ['DataTransform', 'purify_measurement_set']


class DataTransform(casa.CasaTransform):
    def _get_table(self, name=None, filename=None, readonly=True):
        """ A table object """
        # can't use standard import since it would identify pyrap as this very
        # module, rather than the global pyrap module.
        tables = __import__('pyrap.tables', globals(), locals(), ['table'], 0)
        if filename is None:
            filename = self.measurement_set
        if name is None:
            return tables.table(filename, readonly=readonly)
        return tables.table("%s::%s" % (filename, name), readonly=readonly)

    def _c_vs_fortran(self, value):
        """ Function to derive in pyrap wrappers """
        return value.T


def purify_image(datatransform, imagename, imsize=(128, 128), overwrite=False,
                 coordsys=None, weights=None, padmm=False, **kwargs):
    """ Creates an image from a given measurement set

        Parameters:
            ms: str
                Path to the measurement set
            imagename: str
                Path to the output CASA image
            overwrite: bool
                Whether to overwrite `imagename` if it already exists. Defaults
                to False.
            padmm: bool
                If True, selects padmm algorithm
            *args, **kwargs:
                See :py:class:`purify.casa.CasaTransform` and
                :py:func:`purify.casa.purify_measurement_set`
    """
    # Wraps around generators in purify.casa and makes them use the
    # DataTransform employed above, rather than the purify.casa one.
    from numpy import real, array
    from os.path import abspath
    from .casa import set_image_coordinate
    from . import SDMM, PADMM
    # can't use standard import since it would identify pyrap as this very
    # module, rather than the global pyrap module.
    images = __import__('pyrap.images', globals(), locals(), ['image'], 0)

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)

    scale = kwargs.pop('scale', 'default')
    sdmm = (PADMM if padmm else SDMM)(image_size=imsize, **kwargs)
    data = array(
        [real(sdmm(x, scale=scale, weights=weights))
         for x in datatransform],
        dtype='double'
    )
    imagename = abspath(imagename)
    # argument "value = something" seems to be broken
    image = images.image(
        abspath(imagename),
        shape=data.shape,
        overwrite=overwrite,
        coordsys=coordsys
    )
    image.put(data)

    set_image_coordinate(datatransform, abspath(imagename))
    return image


def purify_measurement_set(ms, imagename, imsize=(128, 128), datadescid=0,
                           channels=None, column=None, resolution=0.3,
                           **kwargs):
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
        resolution=resolution
    )

    return purify_image(transform, imagename=imagename, imsize=imsize,
                        **kwargs)
