""" Functionality for interfacing CASA and Purify """
__docformat__ = 'restructuredtext en'
__all__ = ['data_iterator']

class DataTransform(object):
    """ Transforms measurement set to something purify understands """
    def __init__(self, measurement_set, datadescid=0, ignoreW=False,
            channels=None, width=None, column=None, noscaling=False):
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
                width: float, None
                    Width (w) of the U, V band, in the same units as in the
                    measurement set. U, V values are shifted back to the
                    interval :math:`[-w, w[`, via a call to
                    :py:func:`numpy.fmod`.

                    Purify takes as input value in the range
                    :math:`[-\pi, \pi[`. This implies that width in the image
                    domain is :math:`pi/w`.

                    If None, defaults to the :math:`\max|u|` and
                    :math:`\max|v|`.
                noscaling: bool
                    Does not scale U, V to [-pi, pi[. Defaults to False.
        """
        super(DataTransform, self).__init__()

        self.measurement_set = measurement_set
        """ Measurement set for which to get the data """
        self.datadescid = datadescid
        """ Data descriptor id for which to select data """
        self.channels = channels
        """ Channels over which to iterate. Defaults to all. """
        self.column = self._get_column(column)
        """ Name of the data column from which y is taken """
        self.ignoreW = ignoreW
        """ By default, fails if W terms are not zero """
        self.width = width
        """ Width of the U, V bands """
        self.noscaling = noscaling
        """ Does not scale U, V to [-pi, pi[ """

    @property
    def spectral_window_id(self):
        """ ID of the spectral window """
        from taskinit import gentools
        tb, = gentools(['tb'])
        tb.open('%s/DATA_DESCRIPTION' % self.measurement_set)
        return tb.getcol('SPECTRAL_WINDOW_ID')[self.datadescid]

    @property
    def _all_frequencies(self):
        """ Frequencies (Hz) associated with each channel """
        from taskinit import gentools
        tb, = gentools(['tb'])
        tb.open('%s/SPECTRAL_WINDOW' % self.measurement_set)
        return tb.getcol('CHAN_FREQ')[:, self.spectral_window_id].squeeze()

    @property
    def frequencies(self):
        """ Frequencies (Hz) associated with each channel """
        return self._all_frequencies[self.channels].squeeze()

    @property
    def data(self):
        """ Data needed by Purify """
        from numpy import allclose, require
        from taskinit import gentools
        ms, = gentools(['ms'])
        ms.open(self.measurement_set)
        ms.selectinit()
        ms.selectpolarization('I')
        data = ms.getdata([self.column, 'UVW'])
        y = data[self.column.lower()]
        y = require(y, 'complex' if str(y.dtype)[:7] == 'complex' \
                else 'double')
        u, v, w = require(data['uvw'], 'double', 'C_CONTIGUOUS')

        if not (self.ignoreW or allclose(w, 0)):
            raise NotImplementedError('Cannot purify data with W components')

        u, v = self._convert(u), self._convert(v)
        return u, v, y[..., self.channels, :]


    def _convert(self, x):
        """ Normalizes x and puts is in interval [-pi, pi] """
        from numpy import pi, abs, max, fmod
        if self.noscaling: return x
        width = self.width
        if width is None: width = max(abs(x))
        return fmod(x * pi / width, pi)


    def _get_column(self, column):
        """ Name of the column to use for Y """
        from taskinit import gentools

        # open and select in ms
        tb, = gentools(['tb'])
        # Check what's in the table
        tb.open(self.measurement_set)
        columns = tb.colnames()
        if column is not None:
            if column not in columns:
                raise ValueError('Unknown column %s' % column)
            return column
        elif 'CORRECTED_DATA' in columns: return 'CORRECTED_DATA'
        elif 'DATA' in columns: return 'DATA'
        else: raise ValueError('Could determine which column to use for y')

    def _get_channels(self, channels):
        """ Channels to use """
        from taskinit import gentools
        tb, = gentools(['tb'])
        tb.open('%s/SPECTRAL_WINDOW' % self.measurement_set)
        frequencies = tb.getcol('CHAN_FREQ')[:, self.spectral_window_id]
        if channels is None: return list(range(len(frequencies)))
        if hasattr(channels, '__iter__'): channels = list(channels)
        else: channels = [int(channels)]
        if len(channels) == 0: return  list(range(len(frequencies)))
        if any(u >= len(frequencies) for u in channels)    \
                or any(u < -len(frequencies) for u in channels):
            raise IndexError("Channels out of range")
        return channels

def purified_iterator(measurement_set, imsize=(128, 128), datadescid=0,
            ignoreW=False, channels=None, column=None, width=None,
            noscaling=False, **kwargs):
    """ Iterates over purified channels

        Parameters:
            measurement_set:
                Path to the measurement set
            channels: sequence, int, None
                sequence of channels to purify. Defaults to None, which will
                include all channels in the measurement set for the given
                datadescid.
            datadescid: int
                Subset of the measurement set to purify
            column: str, None
                Measurement set column to use for data values. If None, will
                use "CORRECTED_DATA" if it exists, and "DATA" otherwise.
            ignoreW: bool
                If True, fails when w is not zero.
            imsize: (int, int)
                Size of the image in pixels
            width: float, None
                Width (w) of the U, V band, in the same units as in the
                measurement set. U, V values are shifted back to the interval
                :math:`[-w, w[`, via a call to :py:func:`numpy.fmod`.

                Purify takes as input value in the range :math:`[-\pi, \pi[`.
                This implies that width in the image domain is :math:`pi/w`.

                If None, defaults to the :math:`\max|u|` and :math:`\max|v|`.
            noscaling: bool
                Does not scale U, V to [-pi, pi[. Defaults to False.
    """
    from . import SDMM
    iterator = data_iterator(measurement_set, channels=channels,
            datadescid=datadescid, column=column, ignoreW=ignoreW, width=width,
            noscaling=noscaling)
    sdmm = SDMM(image_size=imsize, **kwargs)

    for u, v, y in iterator: yield sdmm((u, v, y))

def data_iterator(*args, **kwargs):
    """ Iterates over channel data """
    transform = DataTransform(*args, **kwargs)
    u, v, y = transform.data
    for i in range(y.shape[-2]): yield u, v, y[..., i, :].squeeze()

# Input for data_iterator and purified_iterator are the same
data_iterator.__doc__ += "\n".join(purified_iterator.__doc__.splitlines()[1:])
