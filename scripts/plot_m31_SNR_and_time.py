import os 
import multiprocessing
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt

def run_test((i, kernel, M_N_ratio)):
	J = 4
	oversample = 2
	if kernel == "pswf":
  		J = 6
	if kernel == "kb_interp":
  		oversample = 1.375
  	os.system("../build/cpp/example/generate_vis_data " + str(M_N_ratio) + " " + str(i)) 
  	start_time = time.time()
  	os.system("../build/cpp/example/sdmm_m31_simulation " + kernel + " " 
      + str(oversample) + " " +str(J) + " " + str(i))
  	end_time = time.time()
  	residual = "../build/outputs/M31_residual_" + kernel + "_" + str(i) + ".fits"
  	solution = "../build/outputs/M31_solution_" + kernel + "_" + str(i) + ".fits"
  	r = np.linalg.norm(pyfits.open(residual)[0].data.flatten())
	s = np.linalg.norm(pyfits.open(solution)[0].data.flatten())
	SNR = 20 * np.log10(s / r)
	os.system("rm " + residual)
	os.system("rm " + solution)
  	return [SNR, end_time - start_time]


if __name__ == '__main__':
	M_N_ratios = np.arange(1, 11) * 0.2
	args = []
	for i in range(1,49):
		for k in ["kb", "kb_interp", "pswf", "gauss"]:
			for m in M_N_ratios:
				args.append((i, k, m))
	p = multiprocessing.Pool(multiprocessing.cpu_count())
	results = p.map(run_test, args)
for i in range(0, len(args)):
	print args[i], results[i]