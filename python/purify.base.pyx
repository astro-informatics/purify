from purify.sensing cimport SensingOperator

class ProximalMinimizationBase(object):
    """ Some functions to normalize input across SDMM, PADMM """

    def __int__(self, **kwargs):
        super(ProximalMinimizationBase, self).__init__()

    def sensing_operator(self, visibility):
        """ Measurement operator used when purifying """
        return SensingOperator(visibility, self.image_size,
                               self.oversampling, self.interpolation)

    def _normalize_input(self, visibility, scale, image, image_dtype=None):
        """ Common input normalization operations """
        from numpy import sqrt, iscomplexobj
        from purify.sensing import visibility_column_as_numpy_array
        # Create missing weights and image objects if needed. Check they are
        # correct otherwise.
        y = visibility_column_as_numpy_array('y', visibility)
        weights_l2 = visibility_column_as_numpy_array('w', visibility)
        if weights_l2 != None and iscomplexobj(weights_l2):
            raise TypeError("L2 weights should not be complex")

        if image_dtype is None:
            image_dtype = y.dtype
        image = self._get_image(image, image_dtype)

        # define all common C objects
        if str(scale).lower() in ('auto', 'default'):
            scale = sqrt(image.size) / sqrt(len(y))
        elif scale is None and str(scale).lower() == 'none':
            scale = None
        else:
            scale = float(scale)

        scaled_y = y * scale if scale is not None else y
        return scaled_y, weights_l2, image, scale

    def _get_image(self, image, dtype):
        """ Check/create input image """
        from numpy import zeros
        if image is not None:
            if image.dtype != dtype:
                image = image.astype(dtype)
            if image.shape != self.image_size:
                raise ValueError(
                    "Shape of input image should be %s, not %s."
                    % (str(self.image_size), str(image.shape))
                )
        else:
            image = zeros(self.image_size, dtype=dtype, order='C')
        return image

    def _get_weight(self, weights):
        """ Check/create input weights """
        from numpy import ones
        wshape = (2 if getattr(self, 'tv_norm', False) else len(self), ) \
            + self.image_size
        if weights is not None:
            if weights.dtype != 'double':
                weights = weights.astype('double')
            if weights.size != wshape:
                message = 'The shape of the weights should be %s' % str(wshape)
                raise ValueError(message)
        else:
            weights = ones(wshape, dtype='double', order='C')
        return weights
