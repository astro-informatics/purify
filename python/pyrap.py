""" Functionality for interfacing CASA and Purify via pyrap """
__docformat__ = 'restructuredtext en'
__all__ = ['data_iterator', 'purified_iterator']
from . import casa

class DataTransform(casa.DataTransform):
    def _get_table(self, name=None):
        """ A table object """
        # can't use standard import since it would identify pyrap as this very
        # module, rather than the global pyrap module.
        tables = __import__('pyrap.tables', globals(), locals(), ['table'], 0)
        if name is None: return tables.table(self.measurement_set)
        return tables.table("%s::%s" % (self.measurement_set, name))
    def _c_vs_fortran(self, value):
        """ Function to derive in pyrap wrappers """
        print value.shape
        return value.T

# Wraps around generators in purify.casa and makes them use the DataTransform
# employed above, rather than the purify.casa one.
def data_iterator(*args, **kwargs):
    kwargs['DataTransform'] = DataTransform
    for o in casa.data_iterator(*args, **kwargs): yield o
data_iterator.__doc__ = casa.data_iterator.__doc__
def purified_iterator(*args, **kwargs):
    kwargs['DataTransform'] = DataTransform
    for o in casa.purified_iterator(*args, **kwargs): yield o
purified_iterator.__doc__ = casa.purified_iterator.__doc__

def purify_image(imagename, *args, **kwargs):
    """ Creates an image from a given measurement set

        Parameters:
            imagename: str
                Path to the output CASA image
            overwrite: bool
                Whether to overwrite `imagename` if it already exists. Defaults
                to False.
            *args, **kwargs:
                See :py:func:`purified_iterator`
    """
    from numpy import real, array
    from os.path import abspath
    # can't use standard import since it would identify pyrap as this very
    # module, rather than the global pyrap module.
    images = __import__('pyrap.images', globals(), locals(), ['image'], 0)

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)
    overwrite = kwargs.pop('overwrite', False)

    data = array(
        [real(x) for x in purified_iterator(*args, **kwargs)],
        dtype = 'double'
    )
    imagename = abspath(imagename)
    return images.image(
        abspath(imagename),
        shape=data.shape,
        values=data,
        overwrite=overwrite
    )
