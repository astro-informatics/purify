""" Functionality for interfacing CASA and Purify """
__docformat__ = 'restructuredtext en'
__all__ = ['purify_measurement_set']


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
        u, v, y, sigma = self.data
        frequencies = self.frequencies

        def conv(x, i):
            isscalar = frequencies.ndim == 0
            frequency = frequencies if isscalar else frequencies[i]
            return self.convert_to_purify(x, frequency, self.resolution)

        for i in range(1 if y.ndim == 1 else y.shape[-2]):
            vis = y[..., i, :].squeeze() if y.ndim != 1 else y
            yield conv(u, i), conv(v, i),\
                vis, sigma[..., i].squeeze()

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
    def __init__(self, measurement_set, datadescid=0, channels=None,
                 resolution=0.3, column=None, **kwargs):
        """ Creates the transform

            :Parameters:
                measurement_set: str
                    Measurement set for which to get the data
                datadescid:
                    Data descriptor id for which to select data
                channels:
                    Channels over which to iterate. Defaults to all.
                column:
                    Name of the data column from which y is taken. Defaults to
                    'CORRECTED_DATA' or 'DATA'.
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
        self.resolution = resolution
        """ Resolution of the output image in arcsec per pixel """

    def _get_table(self, name=None, filename=None, readonly=True):
        """ A table object """
        from taskinit import gentools
        tb, = gentools(['tb'])
        if filename is None:
            filename = self.measurement_set
        if name is None:
            tb.open('%s' % filename, nomodify=readonly)
        else:
            tb.open('%s/%s' % (filename, name), nomodify=readonly)
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
        return self._get_table('DATA_DESCRIPTION') \
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
    def phase_dir(self):
        """ Measurement set phase center """
        return self._c_vs_fortran(self._get_table("FIELD").getcol("PHASE_DIR"))

    @property
    def data(self):
        """ Data needed by Purify """
        from numpy import require, logical_and
        query = 'DATA_DESC_ID == 0'
        columns = "UVW, %s as Y, 1e0/SIGMA as stddev" % self.column
        tb = self._get_table().query(query=query, columns=columns)

        # Compute stokes I component...  this could fail horribly if formula
        # below is incorrect.
        all_ys = self._c_vs_fortran(tb.getcol('Y').squeeze())
        y_is_complex = str(all_ys.dtype)[:7] == 'complex'
        if all_ys.shape[0] != 4:
            msg = "Don't know how to deal with this kind of measurement set"
            raise NotImplementedError(msg)
        yview = all_ys.view()
        yview.shape = all_ys.shape[0], -1
        y = require(0.5 * (yview[0, :] + yview[3, :]),
                    'complex' if y_is_complex else 'double')

        # Remove tagged objects
        stddev = self._c_vs_fortran(tb.getcol('stddev').squeeze())
        if stddev.shape[0] != 4:
            msg = "Don't know how to deal with this kind of measurement set"
            raise NotImplementedError(msg)
        sig_view = stddev.view()
        sig_view.shape = (4, -1)
        unflagged = logical_and(sig_view[0, :] >= 0e0, sig_view[3, :] >= 0e0)

        u, v, stddev = self._c_vs_fortran(tb.getcol('UVW'))
        u, v, stddev, y = u[unflagged], v[unflagged],\
            stddev[unflagged], y[unflagged]

        # Just dealing with CASA difficulties
        channels = None if self.channels == [None] else self.channels
        yview = y.view()
        yview.shape = all_ys.shape[1:-1] + (-1,)
        return u, v, yview[..., channels, :].squeeze(), stddev[..., channels]

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
            return list(range(len(frequencies)))
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


def set_image_coordinate(datatransform, imagename):
    """ Tries and sets an image coordinates from a measurement set """
    from numpy import array, mod, pi
    table = datatransform._get_table(filename=imagename, readonly=False)
    coords = table.getkeyword("coords")
    coords["direction0"]["cdelt"] = \
        array([-1, 1]) * datatransform.resolution * 180 / pi
    coords["direction0"]["crval"] = \
        mod(datatransform.phase_dir[:, 0, 0], 2*pi) * 180 / pi
    table.putkeyword("coords", coords)
    table.close()


def purify_image(datatransform, imagename, imsize=(128, 128), overwrite=False,
                 weights=None, padmm=False, **kwargs):
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
            weights: None or array
                l1weights for sdmm
            padmm: bool
                If True, selects padmm algorithm
            other arguments:
                See purify.SDMM and purify.PADMM
    """
    from os.path import exists, abspath
    from numpy import array
    from taskinit import gentools, casalog
    from . import SDMM, PADMM

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
    algorithm = (PADMM if padmm else SDMM)(image_size=imsize, **kwargs)

    # Contains images over all dimensions
    image = []
    for i, data in enumerate(datatransform):
        casalog.origin('Purifying plane %s' % str(i))
        channel = algorithm(data, scale=scale, weights=weights)
        image.append(channel)

    image = array(image, order='F')

    # Create image
    casalog.post('Creating CASA image')
    ia, = gentools(['ia'])
    ia.newimagefromarray(imagename, image.real.T, overwrite=overwrite)

    set_image_coordinate(datatransform, imagename)


def purify_measurement_set(measurement_set, imagename, imsize=(256, 256),
                           datadescid=0, channels=None, column=None,
                           resolution=0.3, **kwargs):
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
            channels: [Int]
                Channels for which to compute image
            column:
                Column to use for Y data. Defaults to 'CORRECTED_DATA' if
                present, and 'DATA' otherwise.
            other arguments:
                See purify_image
    """
    transform = CasaTransform(
        measurement_set, channels=channels, datadescid=datadescid,
        column=column, resolution=resolution
    )
    return purify_image(transform, imagename=imagename, imsize=imsize,
                        **kwargs)
