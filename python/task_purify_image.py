from purify.casa import purify_measurement_set

def purify_image(vis, imagename, imsize=None, datadescid=0,
        ignoreW=False, channels=None, column=None, resolution=0.3, nlevels=4,
        wavelets=None, interpolation=None, oversampling=None, gamma=None,
        radius=None, relative_radius=None, max_iter=None,
        relative_variation=None, verbose=None, cg_max_iter=None,
        cg_tolerance=None, reweighted=None, rw_max_iter=None, rw_verbose=None,
        rw_sigma=None, rw_relative_variation=None, tv_max_iter=None,
        tv_verbose=None, tv_relative_variation=None, tv_norm=None,
        query="mscal.stokes({0}, 'I')"):
    """ Deals with CASA awkwardness

        Why do things once when you can do 'em twice and thrice as bad?
        Adhering to that particular philosophy, CASA puts all arguments into a
        dictionary *and* manufactures an explicit call with positional
        arguments only. So we have to hand-code a de-awkwarding wrapper of our
        own. The arguments above must be in the same order as those in
        purify.xml.
    """
    try:
        purify_measurement_set(
                measurement_set=vis, imagename=imagename, imsize=imsize,
                datadescid=datadescid, ignoreW=ignoreW, channels=channels,
                column=column, resolution=resolution, nlevels=nlevels,
                wavelets=wavelets, interpolation=interpolation,
                oversampling=oversampling, gamma=gamma, radius=radius,
                relative_radius=relative_radius, max_iter=max_iter,
                relative_variation=relative_variation, verbose=verbose,
                cg_max_iter=cg_max_iter, cg_tolerance=cg_tolerance,
                reweighted=reweighted, rw_max_iter=rw_max_iter,
                rw_verbose=rw_verbose, rw_sigma=rw_sigma,
                rw_relative_variation=rw_relative_variation,
                tv_max_iter=tv_max_iter, tv_verbose=tv_verbose,
                tv_relative_variation=tv_relative_variation, tv_norm=tv_norm,
                query=query
        )
    except:
        from traceback import print_exc
        print_exc()
        raise

# Docstring same for CASA-awkward wrapper and actual implementation
purify_image.__doc__ = purify_measurement_set.__doc__
