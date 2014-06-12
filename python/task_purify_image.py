def purify_image(vis, imagename, imsize=None, datadescid=0, 
        ignoreW=False, select=None, nlevels=None, wavelets=None,
        interpolation=None, oversampling=None, gamma=None, radius=None,
        relative_radius=None, max_iter=None, relative_variation=None,
        verbose=None, cg_max_iter=None, cg_tolerance=None, reweighted=None,
        rw_max_iter=None, rw_verbose=None, rw_sigma=None,
        rw_relative_variation=None, tv_max_iter=None, tv_verbose=None,
        tv_relative_variation=None, tv_norm=None):
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
                datadescid=datadescid, ignoreW=ignoreW,
                select=select, nlevels=nlevels, wavelets=wavelets,
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
        ignoreW=False, select=None, **kwargs):
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
            select: dict
                Dictionary with selection parameters
            ignoreW: boolean
                W is not yet implemented. If W values are non-zero, the call
                will fail unless this parameter is set
            other arguments:
                See purify.SDMM
    """
    from itertools import product
    from numpy import zeros
    from taskinit import gentools, casalog
    from purify import SDMM

    # Input sanitizing
    if 'image_size' in kwargs:
        msg = 'Image size should be given using the imsize argument'
        raise ValueError(msg)

    casalog.post('Starting Purify task')
    y, u, v = _select_data(vis, datadescid, select, ignoreW, casalog)

    # Contains images over all dimensions
    image = zeros(tuple(imsize) + y.shape[:-1], dtype=y.dtype, order='F')

    # Now loop over chain non-image dimension
    sdmm = SDMM(image_size=imsize, **kwargs)
    for indices in product(*[range(x) for x in image.shape[2:]]):
        casalog.origin('Purifying plane %s' % str(indices))
        plane = image.T[indices[::-1]] # .T because CASA wants fortran array
        sdmm((u, v, y[indices]), image=plane)


    # Create image
    casalog.post('Creating CASA image')
    ia, = gentools(['ia'])
    if image.dtype == 'complex':
       ia.newimagefromarray(imagename + ".real", image.real)
       ia.newimagefromarray(imagename + ".complex", image.imag)
    else: ia.newimagefromarray(imagename, image)

# Docstring same for CASA-awkward wrapper and actual implementation
purify_image.__doc__ = _purify_image_impl.__doc__

def _select_data(vis, datadescid, select, ignoreW, casalog):
    """ Open measurement set and select data """
    from numpy import allclose, require
    from taskinit import gentools, casalog

    # open and select in ms
    ms, tb = gentools(['ms', 'tb'])

    casalog.post('Selecting data')
    ms.open(vis)
    ms.selectinit(datadescid)
    if select is not None: ms.select(select)

    # Check what's in the table
    tb.open(vis)
    columns = tb.colnames()

    # Use CORRECTED_DATA by default
    data_name = 'CORRECTED_DATA' if 'CORRECTED_DATA' in columns else 'DATA'
    casalog.post('Using data from column %s' % data_name)
    tables = [data_name,  'UVW']

    # Now get the data
    data = ms.getdata(tables)
    y = data[data_name.lower()]
    y = require(y, 'complex' if str(y.dtype)[:7] == 'complex' else 'double')
    u, v, w = require(data['uvw'], 'double', 'C_CONTIGUOUS')

    if not (ignoreW or allclose(w, 0)):
        raise NotImplementedError('Cannot purify data with W components')

    return y, u, v
