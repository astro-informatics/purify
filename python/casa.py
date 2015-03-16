""" Functionality for interfacing CASA and Purify """
__docformat__ = 'restructuredtext en'
__all__ = ['data_iterator', 'purified_image', 'purify_measurement_set']

class DataTransform(object):
    """ Base class to transform data to something purify understands """
    def __init__(self, u, v, y, frequencies=1, resolution=0.3, **kwargs):
        """ Creates the transform

            :Parameters:
                u: 1-d numpy array
                v: 1-d numpy array
                y: n by x numpy array
                  n is the number of channels, and x the number of visibilities
                frequencies: array
                   For each channel. Defaults to ones.
                resolution: float
                   Arcsec per pixel
        """
        from numpy import require
        super(DataTransform, self).__init__()

        y_is_complex = str(y.dtype)[:7] == 'complex'
        y = require(y, 'complex' if y_is_complex else 'double').squeeze()

        self._data = require(u, 'double'), require(v, 'double'), y
        self._frequencies = require(frequencies, 'double')
        self.resolution = resolution
        """ Resolution of the output image in arcsec per pixel """

    @property
    def frequencies(self):
        """ Frequencies (Hz) associated with each channel """
        return self._frequencies

    @property
    def data(self):
        """ Data needed by Purify """
        return self._data

    def __iter__(self):
        """ Iterates over channel data """
        # Can use a slightly different DataTransform object in pyrap interface
        u, v, y = self.data
        frequencies = self.frequencies

        def conv(x, i):
            isscalar = frequencies.ndim == 0
            frequency = frequencies if isscalar else frequencies[i]
            return self.convert_to_purify(x, frequency, self.resolution)

        for i in range(1 if y.ndim == 1 else y.shape[-2]):
            vis = y[..., i, :].squeeze() if y.ndim != 1 else y
            yield conv(u, i), conv(v, i), vis


    @staticmethod
    def convert_to_purify(x, frequency, resolution=0.3):
        """ Transforms x to purify units given frequency and resolution

            :Parameters:

                x: array
                    Input to convert. Should be in same units as the input
                    frequency.
                frequency: float
                    Frequency of the current channel. Should Hz or equivalent.
                resolution: float
                    Resolution of a pixel, in arc-seconds. Defaults to 0.3
        """
        try:
            from scipy.constants import c
        except ImportError:
            c = 299792458
        from numpy import pi, require
        x = require(x, 'double')
        # Factor to convert to wavelength units
        wavelength = frequency / c
        resolution_rad = resolution / (60.0 * 60.0 * 180.0) * pi
        umax = 1.0 / (2.0 * resolution_rad)
        scale = wavelength * 2.0 * pi / umax
        return x * scale

class CasaTransform(DataTransform):
    """ Transforms measurement set to something purify understands """
    def __init__(self, measurement_set, datadescid=0, ignoreW=False,
            channels=None, resolution=0.3, column=None,
            query="mscal.stokes({0}, 'I')", **kwargs):
        """ Creates the transform

            :Parameters:
                measurement_set: str
                    Measurement set for which to get the data
                datadescid:
                    Data descriptor id for which to select data
                ignoreW:
                    W is not yet implemented. By default, if w are present and
                    non-zero, the call will fail
                channels:
                    Channels over which to iterate. Defaults to all.
                column:
                    Name of the data column from which y is taken. Defaults to
                    'CORRECTED_DATA' or 'DATA'.
                query:
                    Query sent to figure out Y. It is a format string taking
                    one argument, and resolving to a TaQL query. It defaults to
                    "mscal.stokes({0}, 'I')", where %s is the argument. It is
                    substituted wiht the value from column (e.g.
                    'CORRECTED_DATA' or 'DATA').
        """
        super(DataTransform, self).__init__()

        self.measurement_set = measurement_set
        """ Measurement set for which to get the data """
        self.datadescid = datadescid
        """ Data descriptor id for which to select data """
        self.channels = channels
        """ Channels over which to iterate. Defaults to all. """
        self.column = self._get_data_column_name(column)
        """ Name of the data column from which y is taken """
        self.ignoreW = ignoreW
        """ By default, fails if W terms are not zero """
        self.resolution = resolution
        """ Resolution of the output image in arcsec per pixel """
        self.query = query
        """ Query from which Y is obtained.

            It is a format string taking one argument, and resolving to a TaQL
            query. It defaults to  "mscal.stokes({0}, 'I')", where %s is the
            argument. It is substituted wiht the value from column (e.g.
            'CORRECTED_DATA' or 'DATA').
        """


    def _get_table(self, name=None):
        """ A table object """
        from taskinit import gentools
        tb, = gentools(['tb'])
        if name is None:
            tb.open('%s' % self.measurement_set)
        else:
            tb.open('%s/%s' % (self.measurement_set, name))
        return tb

    def _c_vs_fortran(self, value):
        """ Function to derive in pyrap wrappers 

            Different indexing conventions are used in CASA(Fortran) and
            pyrap(C).
        """
        return value

    @property
    def spectral_window_id(self):
        """ ID of the spectral window """
        return self._get_table('DATA_DESCRIPTION')\
                .getcol('SPECTRAL_WINDOW_ID')[self.datadescid]

    @property
    def _all_frequencies(self):
        """ Frequencies (Hz) associated with each channel """
        return self._get_table('SPECTRAL_WINDOW')\
                .getcol('CHAN_FREQ')[:, self.spectral_window_id].squeeze()

    @property
    def frequencies(self):
        """ Frequencies (Hz) associated with each channel """
        from numpy import ones
        allfreqs = self._all_frequencies
        if allfreqs.ndim == 0:
            condition = self.channels is None or self.channels == [None]
            channels = [0] if condition else self.channels
            allfreqs = ones(max(channels) + 1, dtype='double') * allfreqs
        return allfreqs[self.channels].squeeze()

    @property
    def data(self):
        """ Data needed by Purify """
        from numpy import allclose, require
        query = 'DATA_DESC_ID == 0'
        columns = "UVW, %s as Y" % self.query.format(self.column)
        tb = self._get_table().query(query=query, columns=columns)
        y = self._c_vs_fortran(tb.getcol('Y').squeeze())
        y_is_complex = str(y.dtype)[:7] == 'complex'
        y = require(y, 'complex' if y_is_complex else 'double')
        u, v, w = self._c_vs_fortran(tb.getcol('UVW'))

        if not (self.ignoreW or allclose(w, 0)):
            raise NotImplementedError('Cannot purify data with W components')

        # Just dealing with CASA difficulties
        channels = None if self.channels == [None] else self.channels
        return u, v, y[..., channels, :].squeeze()

    def _get_data_column_name(self, column):
        """ Name of the column to use for Y """
        columns = self._get_table().colnames()
        if column is not None:
            if column not in columns:
                raise ValueError('Unknown column %s' % column)
            return column
        elif 'CORRECTED_DATA' in columns:
            return 'CORRECTED_DATA'
        elif 'DATA' in columns:
            return 'DATA'
        else:
            raise ValueError('Could determine which column to use for y')

    def _get_channels(self, channels):
        """ Channels to use """
        frequencies = self._all_frequencies
        if channels is None:
            return list(range(len(frequencies)))
        if hasattr(channels, '__iter__'):
            channels = list(channels)
        else:
            channels = [int(channels)]
        if len(channels) == 0:
            return  list(range(len(frequencies)))
        if any(u >= len(frequencies) for u in channels)    \
                or any(u < -len(frequencies) for u in channels):
            raise IndexError("Channels out of range")
        return channels

class LambdaTransform(CasaTransform):
    def __init__(self, measurement_set, resolution=0.3, norm=1.0, **kwargs):
        from numpy import loadtxt
        self.resolution = resolution
        data = loadtxt(measurement_set)
        u, v, y = data[:, 0], data[:, 1], data[:, 2] + 1.0j * data[:, 3]
        self._data = u, v, y.reshape(1, len(y)) / float(norm)
        self.resolution = resolution
        """ Resolution of the output image in arcsec per pixel """

    @property
    def data(self):
        return self._data

    @property
    def frequencies(self):
        from numpy import array
        return array(1.0)

    @staticmethod
    def convert_to_purify(x, frequency, resolution=0.3):
        """ Transforms x to purify units given frequency and resolution

            :Parameters:

                x: array
                    Input to convert. Should be in same units as the input
                    frequency.
                frequency: float
                    Frequency of the current channel. Should Hz or equivalent.
                resolution: float
                    Resolution of a pixel, in arc-seconds. Defaults to 0.3
        """
        from numpy import pi, require
        x = require(x, 'double')
        # Factor to convert to wavelength units
        resolution_rad = resolution / (60.0 * 60.0 * 180.0) * pi
        umax = 1.0 / (2.0 * resolution_rad)
        scale = 2.0 * pi / umax
        return x * scale

def purify_image(datatransform, imagename, imsize=(128, 128), overwrite=False,
        **kwargs):
    """ Creates an image using the Purify method

        Parameters:
            datatransform: DataTransform derived
                Iterator of purify data
            imagename: string
                Path to output image
            imsize: 2-tuple
                Width and height of the output image in pixels
            overwrite: bool
                Whether to overwrite existing image. Defaults to False.
            other arguments:
                See purify.SDMM
    """
    from os.path import exists, abspath
    from numpy import array
    from taskinit import gentools, casalog
    from . import SDMM

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)

    imagename = abspath(imagename)
    if exists(imagename) and not overwrite:
        msg = "File %s already exist. It will not be overwritten.\n" \
                "Please delete the file or choose another filename for the" \
                " image."
        raise IOError(msg % imagename)

    casalog.post('Starting Purify task')
    scale = kwargs.pop('scale', 'default')
    sdmm = SDMM(image_size=imsize, **kwargs)


    # Contains images over all dimensions
    image = []
    for i, data in enumerate(datatransform):
        casalog.origin('Purifying plane %s' % str(i))
        channel = sdmm(data, scale=scale)
        image.append(channel)

    image = array(image, order='F')

    # Create image
    casalog.post('Creating CASA image')
    ia, = gentools(['ia'])
    ia.newimagefromarray(imagename, image.real, overwrite=overwrite)

def purify_measurement_set(measurement_set, imagename, imsize=None,
        datadescid=0, ignoreW=False, channels=None, column=None,
        resolution=0.3, query="mscal.stokes({0}, 'I')", **kwargs):
    """ Creates an image using the Purify method

        Parameters:
            measurement_set: string
                Path to the measurement set with the data to purify
            imagename: string
                Path to output image
            imsize: 2-tuple
                Width and height of the output image in pixels
            datadescid: int
                Integer selecting the subset of consistent data
            ignoreW: boolean
                W is not yet implemented. If W values are non-zero, the call
                will fail unless this parameter is set
            channels: [Int]
                Channels for which to compute image
            column:
                Column to use for Y data. Defaults to 'CORRECTED_DATA' if
                present, and 'DATA' otherwise.
            query:
                Query sent to figure out Y. It is a format string taking one
                argument, and resolving to a TaQL query. It defaults to
                "mscal.stokes({0}, 'I')", where %s is the argument. It is
                substituted wiht the value from column (e.g. 'CORRECTED_DATA'
                or 'DATA').
            other arguments:
                See purify_image
    """
    transform = CasaTransform(
        measurement_set, channels=channels, datadescid=datadescid,
        column=column, ignoreW=ignoreW, resolution=resolution, query=query
    )
    return purify_image(
            transform, imagename=imagename, imsize=imsize, **kwargs)
