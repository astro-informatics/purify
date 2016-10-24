import astropy.io.fits as fits
import numpy as np

speed_of_light = 299792458. #m/s

def readData(filename, vis_name, pol):
    hdu = fits.open(filename)
    #reading in uvdata
    print "Header..."
    print hdu[0].header
    print hdu[0].data['DATA'].shape

    no_chan = hdu[0].data['DATA'].shape[3]
    no_pol =  hdu[0].data['DATA'].shape[4]

    re = np.array([])
    im = np.array([])
    sigma = np.array([])
    u = np.array([])
    v = np.array([])
    for chan in range(no_chan):
        freq = hdu[0].header["CRVAL4"] + chan * hdu[0].header["CDELT4"]
        
        if (np.count_nonzero(hdu[0].data['DATA'][:, 0, 0, chan, pol, 0]**2 + hdu[0].data['DATA'][:, 0, 0, chan, pol, 1]**2) > 0):
            print "loading frequency ", freq, chan
            flags = hdu[0].data['DATA'][:, 0, 0, chan, pol, 2]/np.abs(hdu[0].data['DATA'][:, 0, 0, chan, pol, 2]) #I think this gives the flags....
            sigma = np.concatenate((sigma, 1./np.sqrt(hdu[0].data['DATA'][:, 0, 0, chan, pol, 2][flags > 0])))
            re = np.concatenate((re, hdu[0].data['DATA'][:, 0, 0, chan, pol, 0][flags > 0]))
            im = np.concatenate((im, hdu[0].data['DATA'][:, 0, 0, chan, pol, 1][flags > 0]))
            #reading in uv-coordinates
            u = np.concatenate((u, hdu[0].data['UU'][flags > 0] * freq))
            v = np.concatenate((v, hdu[0].data['VV'][flags > 0] * freq))
    print "Total visibilities... ", u.shape, v.shape, re.shape, im.shape, sigma.shape
    #reading in weights
    table = np.column_stack((u, v, re, im, sigma))
    print table[0,:], u[0], v[0], re[0], im[0], sigma[0]
    np.savetxt(vis_name, table, delimiter = " ")

names = ["0332-391", "0114-476", "1637-77", "sumss0515"]
for i in range(len(names)):
    uv_fits = "/Users/luke/Radio_Data/" + names[i] +".uvfits"
    output_vis = names[i] + ".vis"
    readData(uv_fits, output_vis, 0)

