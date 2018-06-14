import scipy.misc
import pyfits

og_image_hdu = pyfits.open("../data/images/M31.fits")
og_image = og_image_hdu[0].data[0]
print og_image.shape
image_512 = scipy.misc.imresize(og_image, 2.)
pyfits.writeto("M31_512.fits", image_512)
print image_512.shape
image_1024 = scipy.misc.imresize(og_image, 4.)
pyfits.writeto("M31_1024.fits", image_1024)
print image_1024.shape
image_2048 = scipy.misc.imresize(og_image, 8.)
pyfits.writeto("M31_2048.fits", image_2048)
print image_2048.shape
image_4096 = scipy.misc.imresize(og_image, 16.)
pyfits.writeto("M31_4096.fits", image_4096)
print image_4096.shape