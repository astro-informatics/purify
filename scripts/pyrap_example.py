from os.path import dirname, join, exists
from purify.pyrap import purify_measurement_set

# input measurement set is created in separate script.
# This script requires CASA
ms = 'ngc4826.tutorial.16apr98.src.split.ms'
if not exists(ms):
    filename = join(dirname(__file__), 'get_data.py')
    msg = '%s does not exist.\nPlease run casapy %s first\n' % (ms, filename)
    raise RuntimeError(msg)

image = purify_measurement_set(
    imagename = 'outputimg',
    measurement_set = ms,
    overwrite = True,
    ignoreW = True,
    max_iter = 10,
    nlevels = 5,
    wavelets = ['DB%i' % i for i in range(1, 10)],
    channels = [35, 36],
    imsize = (128, 128),
    resolution = 0.1 # arcsec per pixel
)
