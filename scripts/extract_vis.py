# -------------------------------------------------
#    Script for making vis-files from UVFITS/MS
#    Assumes CASA Version 4.1.0
# -------------------------------------------------
# [130902] - a.scaife@soton.ac.uk
# [151012] - luke.pratley.15@ucl.ac.uk
# -------------------------------------------------
# to run:
# my-laptop> casapy --nologger -c extract_vis.py
# -------------------------------------------------


import numpy as np
import os,sys

const_c = 299792458. # m/s


# -----------------------------------------------------
# define input data files:
infile='CygA-X'


# -----------------------------------------------------
# open MS SPECTRAL table:
tb.open(infile+'.MS/SPECTRAL_WINDOW',nomodify=True)
# extract channel frequencies (in Hz):
freq=tb.getcol("CHAN_FREQ")
refreq=tb.getcol("REF_FREQUENCY")
print ">>Extracting data for ",freq.shape[1]," frequencies: ",freq/1e9,"[GHz]"
tb.close


# -----------------------------------------------------
# open MS MAIN table:
tb.open(infile+'.MS',nomodify=True)
# extract UVW (note these are in metres):
uvw=tb.getcol("UVW")
units=tb.getcolkeyword("UVW", "QuantumUnits")
print ">>Units of UVW are: ",units[0]
print ">>There are ",uvw.shape[1]," visibilities per channel"
# extract complex data:
data=tb.getcol("DATA")
# extract weight for each visibility:
sigma=tb.getcol("SIGMA")

# -----------------------------------------------------
# open output vis-file:
os.system('rm -rf '+infile+'.vis \n')
visfile=open(infile+'.vis','w')
print 'Total Channels: ', len(data[0,:,0])
# Loop over frequency channels, and add uv data to .vis file
for j in range(0,len(data[0,:,0])):
	print 'Adding Channel:', j, 'Frequency (Hz):', freq[j, 0]
	flag = tb.getcol('FLAG')[0, j, :]

	dataI = 0.5*(data[0,j,:]+data[3,j,:])
	sigmaI= 0.5*np.sqrt(sigma[0,:]**2+sigma[3,:]**2)

	# -----------------------------------------------------
	print ">> Columns are:"
	print " U(lambda)  V(lambda)  Real(Jy)  Imag(Jy)  Sigma(Jy) "

	uvw_c = uvw*np.mean(freq)/const_c # convert from m -> lambda, not wanted for MFS!
	#scale = 1./(60. * 60. * 5)
	#uvw_c = uvw_c * scale
	# loop over visibility:
	for i in range(0,uvw.shape[1]):
	    if (sigma[0,i]!=-1.0)and(sigma[3,i]!=-1.0)and(flag[i]==0):
	        visfile.write(str(uvw_c[0,i])+' '+str(uvw_c[1,i])+' '\
	                    +str(dataI[i].real)+' '+str(dataI[i].imag)\
	                    +' '+str(sigmaI[i])+'\n')
	break


visfile.close()


tb.close
