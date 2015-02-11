# This script is run by first launching casapy and then doing:
# >>> run -i get_data.py
# >>> run -i casa_example.py
# The script will fail if outputimg already exists
from os.path import dirname, join, exists
# Importing purify register the CASA task
import purify


# input measurement set is created in separate script.
# This script requires CASA
ms = 'ngc4826.tutorial.16apr98.src.split.ms'
if not exists(ms):
    filename = join(dirname(__file__), 'get_data.py')
    msg = '%s does not exist.\nPlease run casapy %s first\n' % (ms, filename)
    raise RuntimeError(msg)


default('purify_image')
vis = ms
imagename = 'outputimg'
ignoreW = True
max_iter = 10
nlevels = 5
wavelets = ['DB%i' % i for i in range(1, 10)]
channels = [35, 36]
resolution = 0.1 # arcsec per pixel
imsize = 128, 128
purify_image()
