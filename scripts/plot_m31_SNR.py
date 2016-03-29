#!/usr/bin/python

import numpy as np 
import pyfits
import glob
import os
import subprocess
import matplotlib.pyplot as plt

def calculate_average_SNR(kernel):
	residuals = glob.glob("../build/outputs/*residual*" + kernel + "*.fits")
	solutions = glob.glob("../build/outputs/*solution*" + kernel + "*.fits")
	SNRs = np.zeros(len(residuals))
	for i in range(0, len(SNRs)):
		r = np.linalg.norm(pyfits.open(residuals[i])[0].data.flatten())
		s = np.linalg.norm(pyfits.open(solutions[i])[0].data.flatten())
		SNRs[i] = 20 * np.log10(s / r)
		os.system("rm " + residuals[i])
		os.system("rm " + solutions[i])
	return SNRs.mean(), SNRs.std()

meankbSNR = []
meankb_interpSNR = []
meangaussSNR = []
meanpswfSNR = []
errorkbSNR = []
errorkb_interpSNR = []
errorgaussSNR = []
errorpswfSNR = []
M_over_N = np.arange(1, 11) * 0.2
for i in M_over_N:
	process = subprocess.Popen(["python", "run_m31_test.py", "kb", str(i)])
	process.wait()
	snr, error = calculate_average_SNR("kb")
	meankbSNR.append(snr)
	errorkbSNR.append(error)
for i in M_over_N:
	process = subprocess.Popen(["python", "run_m31_test.py", "kb_interp", str(i)])
	process.wait()
	snr, error = calculate_average_SNR("kb_interp")
	meankb_interpSNR.append(snr)
	errorkb_interpSNR.append(error)
for i in M_over_N:
	process = subprocess.Popen(["python", "run_m31_test.py", "gauss", str(i)])
	process.wait()
	snr, error = calculate_average_SNR("gauss")
	meangaussSNR.append(snr)
	errorgaussSNR.append(error)
for i in M_over_N:
	process = subprocess.Popen(["python", "run_m31_test.py", "pswf", str(i)])
	process.wait()
	snr, error = calculate_average_SNR("pswf")
	meanpswfSNR.append(snr)
	errorpswfSNR.append(error)
print len(meankbSNR), len(errorkbSNR)
plt.errorbar(M_over_N, meankbSNR, errorkbSNR, fmt='')
plt.errorbar(M_over_N, meankb_interpSNR, errorkb_interpSNR, c = "red", fmt='')
plt.errorbar(M_over_N, meangaussSNR, errorgaussSNR, c = "green", fmt='')
plt.errorbar(M_over_N, meanpswfSNR, errorpswfSNR, c = "black", fmt='')
plt.legend(["kb", "kb_interp", "gauss", "pswf"])
plt.xlabel("M/N")
plt.ylabel("SNR, db")
plt.savefig("example_SNR_plot.png")
