import astropy.io.fits as fits
import numpy as np

speed_of_light = 299792458. #m/s

def readData(filename, vis_name, pol1, pol2, filter):
    hdu = fits.open(filename)
    data = np.asfortranarray(hdu[0].data['DATA'])
    #reading in uvdata
    print "Header..."
    #   print hdu[0].header
    print hdu[0].data['DATA'].shape
    no_chan = data.shape[3]
    no_pol =  data.shape[4]

    re = np.array([])
    im = np.array([])
    sigma = np.array([])
    u = np.array([])
    v = np.array([])
    w = np.array([])
    for chan in range(no_chan):
        freq = hdu[0].header["CRVAL4"] + chan * hdu[0].header["CDELT4"]

        print "Loading frequency ", freq, chan
        flags1 = data[:, 0, 0, chan, pol1, 2] #I think this gives the flags....
        flags2 = data[:, 0, 0, chan, pol2, 2] #I think this gives the flags....
        flags = np.logical_or(np.logical_and((flags1> 0), (flags2 > 0)), not filter)
        print "Flagged visibilities: ", + (~flags).sum()
        #reading in weights and visibilities
        sigma = np.concatenate((sigma, np.sqrt((1/data[:, 0, 0, chan, pol1, 2][flags]) /2 + (1/data[:, 0, 0, chan, pol2, 2][flags]) /2)))
        re = np.concatenate((re, data[:, 0, 0, chan, pol1, 0][flags]/2 + data[:, 0, 0, chan, pol2, 0][flags]/2))
        im = np.concatenate((im, data[:, 0, 0, chan, pol1, 1][flags]/2 + data[:, 0, 0, chan, pol2, 1][flags]/2))
        print data[:, 0, 0, chan, pol2, 2][~flags]
        #reading in uv-coordinates
        u = np.concatenate((u, hdu[0].data['UU'][flags] * freq))
        v = np.concatenate((v, hdu[0].data['VV'][flags] * freq))
        w = np.concatenate((w, hdu[0].data['WW'][flags] * freq))
        print "Total visibilities... ", u.shape, v.shape, w.shape, re.shape, im.shape, sigma.shape

    if filter:
        remove_nan = np.logical_and(~np.isnan(re) , ~np.isnan(complex(0, 1) *im))
        u = u[remove_nan]
        v = v[remove_nan]
        w = w[remove_nan]
        re = re[remove_nan]
        im = im[remove_nan]
        sigma = sigma[remove_nan]
        remove_zero = np.abs(re + complex(0, 1) *im) > 0
        u = u[remove_zero]
        v = v[remove_zero]
        w = w[remove_zero]
        re = re[remove_zero]
        im = im[remove_zero]
        sigma = sigma[remove_zero]

    table = np.column_stack((u, v, w, re, im, sigma))
    print table[0,:], u[0], v[0], w[0], re[0], im[0], sigma[0]
    print "Total visibilities... ", u.shape, v.shape, w.shape, re.shape, im.shape, sigma.shape

    np.savetxt(vis_name, table, delimiter = " ")

names = ["0332-391"]
for i in range(len(names)):
    uv_fits = "/Users/luke/Radio_Data/" + names[i] +".uvfits"
    output_vis = names[i] + ".vis"
    readData(uv_fits, output_vis, 0, 1, True)
    print "saved "+ names[i] + ".vis"
