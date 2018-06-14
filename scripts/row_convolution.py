import numpy as np
import scipy.signal
g_row = np.random.random((5, 5))
c_row = np.random.random((5, 5))
g_row_shift = np.fft.ifftshift(np.fliplr(np.flipud(g_row)))
c_row_shift = np.fft.ifftshift(np.fliplr(np.flipud(c_row)))

print g_row
print c_row

k_row_shift = scipy.signal.convolve2d(g_row_shift, c_row_shift,'same')

k_row = np.flipud(np.fliplr(np.fft.fftshift(k_row_shift)))

print np.array(g_row.flatten('C'))
print np.array(c_row.flatten('C'))
print np.array(k_row.flatten('C'))
