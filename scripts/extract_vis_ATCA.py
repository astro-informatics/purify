# -------------------------------------------------
#    Script for making vis-files from UVFITS/MS
#    Assumes CASA Version 4.1.0
# -------------------------------------------------
# [130902] - a.scaife@soton.ac.uk
# [151012] - luke.pratley.15@ucl.ac.uk
# -------------------------------------------------
# to run:
# my-laptop> casapy --nologger -c extract_vis_ATCA.py
# -------------------------------------------------


import numpy as np
import os,sys

const_c = 299792458. # m/s


# -----------------------------------------------------
# define input data files:
infile='0153-410'


# -----------------------------------------------------
# open MS SPECTRAL table:
tb.open(infile+'.ms/SPECTRAL_WINDOW',nomodify=True)
# extract channel frequencies (in Hz):
freq=tb.getcol("CHAN_FREQ")
refreq=tb.getcol("REF_FREQUENCY")
print ">>Extracting data for ",freq.shape[1]," frequencies: ",freq/1e9,"[GHz]"
tb.close


# -----------------------------------------------------
# open MS MAIN table:
tb.open(infile+'.ms',nomodify=True)
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
os.system('rm -rf '+infile+'*.vis \n')
visfileI=open(infile+'I.vis','w')
visfileQ=open(infile+'Q.vis','w')
visfileU=open(infile+'U.vis','w')
visfileV=open(infile+'V.vis','w')
print 'Total Channels: ', len(data[0,:,0])
# Loop over frequency channels, and add uv data to .vis file
for j in range(0,len(data[0,:,0])):
	print 'Adding Channel:', j, 'Frequency (Hz):', freq[j, 0]
	flagXX = np.array(tb.getcol('FLAG')[0, j, :])
	flagXY = np.array(tb.getcol('FLAG')[1, j, :])
	flagYX = np.array(tb.getcol('FLAG')[2, j, :])
	flagYY = np.array(tb.getcol('FLAG')[3, j, :])
	# Assuming linear feed and correlation matrix of [[0 1], [2 3]]
	dataI = 0.5*(data[0,j,:] + data[3,j,:])
	dataU = 0.5*(data[1,j,:] + data[2,j,:])
	dataV = 0.5*(data[1,j,:] - data[2,j,:])/complex(0 , 1)
	dataQ = 0.5*(data[0,j,:] - data[3,j,:])
	sigmaI = np.sqrt((sigma[0,:]**2 + sigma[3,:]**2)/2.)
	sigmaU = np.sqrt((sigma[1,:]**2 + sigma[2,:]**2)/2.)
	sigmaV = np.sqrt((sigma[1,:]**2 + sigma[2,:]**2)/2.)
	sigmaQ = np.sqrt((sigma[0,:]**2 + sigma[3,:]**2)/2.)

	# -----------------------------------------------------
	print ">> Columns are:"
	print " U(lambda)  V(lambda)  Real(Jy)  Imag(Jy)  Sigma(Jy) "

	uvw_c = uvw * freq[j, 0]/const_c # convert from m -> lambda
	#scale = 1./(60. * 60. * 5)
	#uvw_c = uvw_c * scale
	# loop over visibility:
	for i in range(0,uvw.shape[1]):
	    if (sigma[0,i]!=-1.0)and(sigma[1,i]!=-1.0)and(sigma[2,i]!=-1.0)and(sigma[3,i]!=-1.0)and(flagXX[i]==False)and(flagXY[i]==False)and(flagYX[i]==False)and(flagYY[i]==False):
	        visfileI.write(str(uvw_c[0,i])+' '+str(uvw_c[1,i])+' '\
	                    +str(dataI[i].real)+' '+str(dataI[i].imag)\
	                    +' '+str(sigmaI[i])+'\n')
	        visfileQ.write(str(uvw_c[0,i])+' '+str(uvw_c[1,i])+' '\
	                    +str(dataQ[i].real)+' '+str(dataQ[i].imag)\
	                    +' '+str(sigmaQ[i])+'\n')
	        visfileU.write(str(uvw_c[0,i])+' '+str(uvw_c[1,i])+' '\
	                    +str(dataU[i].real)+' '+str(dataU[i].imag)\
	                    +' '+str(sigmaU[i])+'\n')
	        visfileV.write(str(uvw_c[0,i])+' '+str(uvw_c[1,i])+' '\
	                    +str(dataV[i].real)+' '+str(dataV[i].imag)\
	                    +' '+str(sigmaV[i])+'\n')
	


visfileI.close()
visfileQ.close()
visfileU.close()
visfileV.close()


tb.close
