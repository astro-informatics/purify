#!/usr/bin/python

import numpy as np 
import pyfits
import glob
import os
import subprocess
import matplotlib.pyplot as plt

def calculate_average_SNR():
	residuals = glob.glob("../build/outputs/*residual*.fits")
	solutions = glob.glob("../build/outputs/*solution*.fits")
	SNRs = np.zeros(len(residuals))
	for i in range(0, len(SNRs)):
		r = pyfits.open(residuals[i])[0].data.flatten().norm()
		s = pyfits.open(solutions[i])[0].data.flatten().norm()
		SNRs[i] = 20 * np.log10(s / r)
		os.system("rm " + residuals[i])
		os.system("rm " + solutions[i])
	return SNRs.mean()

meankbSNR = []
meankb_interpSNR = []
meangaussSNR = []
meanpswfSNR = []
M_over_N = np.arange(1, 10) * 0.2
for i in M_over_N:
	process = subprocess.Popen(["python", "run_m31_test.py", "kb", str(i)])
	process.wait()
	meankbSNR.append(calculate_average_SNR())
	# process = subprocess.Popen(["python", "run_m31_test.py", "kb_interp", str(i)])
	# process.wait()
	# meankb_interpSNR.append(calculate_average_SNR())
	# process = subprocess.Popen(["python", "run_m31_test.py", "gauss", str(i)])
	# process.wait()
	# meangaussSNR.append(calculate_average_SNR())
	# process = subprocess.Popen(["python", "run_m31_test.py", "pswf", str(i)])
	# process.wait()
	# meanpswfSNR.append(calculate_average_SNR())

plt.scatter(M_over_N, meankbSNR)
plt.savefig("example_SNR_plot.png")
