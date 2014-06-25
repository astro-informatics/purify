def purify_image(vis, imagename, imsize=None, datadescid=0,
        ignoreW=False, channels=None, column=None, width=None, nlevels=4,
        wavelets=None, interpolation=None, oversampling=None, gamma=None,
        radius=None, relative_radius=None, max_iter=None,
        relative_variation=None, verbose=None, cg_max_iter=None,
        cg_tolerance=None, reweighted=None, rw_max_iter=None, rw_verbose=None,
        rw_sigma=None, rw_relative_variation=None, tv_max_iter=None,
        tv_verbose=None, tv_relative_variation=None, tv_norm=None):
    """ Deals with CASA awkwardness

        Why do things once when you can do 'em twice and thrice as bad?
        Adhering to that particular philosophy, CASA puts all arguments into a
        dictionary *and* manufactures an explicit call with positional
        arguments only. So we have to hand-code a de-awkwarding wrapper of our
        own. The arguments above must be in the same order as those in
        purify.xml.
    """
    try:
        _purify_image_impl(
                vis=vis, imagename=imagename, imsize=imsize,
                datadescid=datadescid, ignoreW=ignoreW, channels=channels,
                column=column, width=width, nlevels=nlevels, wavelets=wavelets,
                interpolation=interpolation, oversampling=oversampling,
                gamma=gamma, radius=radius, relative_radius=relative_radius,
                max_iter=max_iter, relative_variation=relative_variation,
                verbose=verbose, cg_max_iter=cg_max_iter,
                cg_tolerance=cg_tolerance, reweighted=reweighted,
                rw_max_iter=rw_max_iter, rw_verbose=rw_verbose,
                rw_sigma=rw_sigma, rw_relative_variation=rw_relative_variation,
                tv_max_iter=tv_max_iter, tv_verbose=tv_verbose,
                tv_relative_variation=tv_relative_variation, tv_norm=tv_norm
        )
    except:
        from traceback import print_exc
        print_exc()
        raise

def _purify_image_impl(vis, imagename, imsize=None, datadescid=0,
        ignoreW=False, channels=None, column=None, **kwargs):
    """ Creates an image using the Purify method

        Parameters:
            vis: string
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
            other arguments:
                See purify.SDMM
    """
    from os.path import exists, abspath
    from numpy import array
    from taskinit import gentools, casalog
    from purify.casa import purified_iterator

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)

    imagename = abspath(imagename)
    if exists(imagename):
        msg = "File %s already exist. It will not be overwritten.\n" \
                "Please delete the file or choose another filename for the" \
                " image."
        raise IOError(msg % imagename)

    casalog.post('Starting Purify task')
    iterator = purified_iterator(vis, channels=channels,
            datadescid=datadescid, column=column, ignoreW=ignoreW, **kwargs)

    # Contains images over all dimensions
    image = []
    for i, channel in enumerate(iterator):
        casalog.origin('Purifying plane %s' % str(i))
        image.append(channel)

    image = array(image, order='F')

    # Create image
    casalog.post('Creating CASA image')
    ia, = gentools(['ia'])
    # Only the real part is meaningful
    ia.newimagefromarray(imagename, image.real)

# Docstring same for CASA-awkward wrapper and actual implementation
purify_image.__doc__ = _purify_image_impl.__doc__
