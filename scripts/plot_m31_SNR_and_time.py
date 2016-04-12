import os 
import multiprocessing
import random
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt

def run_test((i, kernel, M_N_ratio)):
	time.sleep(random.random())
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
	n_tests = 48
	test_num = 0
	for i in range(1, n_tests + 1):
		for k in ["kb", "kb_interp", "pswf", "gauss"]:
			for m in M_N_ratios:
				test_num = test_num + 1
				args.append((test_num, k, m))
	p = multiprocessing.Pool(multiprocessing.cpu_count())
	results = p.map(run_test, args)
	meankbSNR = []
	meankb_interpSNR = []
	meangaussSNR = []
	meanpswfSNR = []
	errorkbSNR = []
	errorkb_interpSNR = []
	errorgaussSNR = []
	errorpswfSNR = []

	meankbTime = []
	meankb_interpTime = []
	meangaussTime = []
	meanpswfTime = []
	errorkbTime = []
	errorkb_interpTime = []
	errorgaussTime = []
	errorpswfTime = []

	for m in M_N_ratios:
		kbSNR = []
		kb_interpSNR = []
		gaussSNR = []
		pswfSNR = []

		kbTime = []
		kb_interpTime = []
		gaussTime = []
		pswfTime = []
		for i in range(len(args)):
			if m == args[i][2]:
				if args[i][1] == "kb":
					kbSNR.append(results[i][0])
					kbTime.append(results[i][1])
				if args[i][1] == "kb_interp":
					kb_interpSNR.append(results[i][0])
					kb_interpTime.append(results[i][1])
				if args[i][1] == "gauss":
					gaussSNR.append(results[i][0])
					gaussTime.append(results[i][1])
				if args[i][1] == "pswf":
					pswfSNR.append(results[i][0])
					pswfTime.append(results[i][1])
		kbSNR = np.array(kbSNR)
		kb_interpSNR = np.array(kb_interpSNR)
		gaussSNR = np.array(gaussSNR)
		pswfSNR = np.array(pswfSNR)
		
		meankbSNR.append(kbSNR.mean())
		meankb_interpSNR.append(kb_interpSNR.mean())
		meangaussSNR.append(gaussSNR.mean())
		meanpswfSNR.append(pswfSNR.mean())
		
		errorkbSNR.append(kbSNR.std())
		errorkb_interpSNR.append(kb_interpSNR.std())
		errorgaussSNR.append(gaussSNR.std())
		errorpswfSNR.append(pswfSNR.std())

		kbTime = np.array(kbTime)
		kb_interpTime = np.array(kb_interpTime)
		gaussTime = np.array(gaussTime)
		pswfTime = np.array(pswfTime)
		
		meankbTime.append(kbTime.mean())
		meankb_interpTime.append(kb_interpTime.mean())
		meangaussTime.append(gaussTime.mean())
		meanpswfTime.append(pswfTime.mean())
		
		errorkbTime.append(kbTime.std())
		errorkb_interpTime.append(kb_interpTime.std())
		errorgaussTime.append(gaussTime.std())
		errorpswfTime.append(pswfTime.std())
		
	plt.errorbar(M_N_ratios, meankbSNR, errorkbSNR, fmt='')
	plt.errorbar(M_N_ratios, meankb_interpSNR, errorkb_interpSNR, c = "red", fmt='')
	plt.errorbar(M_N_ratios, meangaussSNR, errorgaussSNR, c = "green", fmt='')
	plt.errorbar(M_N_ratios, meanpswfSNR, errorpswfSNR, c = "black", fmt='')
	plt.legend(["kb", "kb_interp", "gauss", "pswf"], loc = "upper left")
	plt.xlabel("M/N")
	plt.ylabel("SNR, db")
	plt.xlim(0,2.2)
	plt.savefig("example_SNR_plot.png")
	plt.clf()

	plt.errorbar(M_N_ratios, meankbTime, errorkbTime, fmt='')
	plt.errorbar(M_N_ratios, meankb_interpTime, errorkb_interpTime, c = "red", fmt='')
	plt.errorbar(M_N_ratios, meangaussTime, errorgaussTime, c = "green", fmt='')
	plt.errorbar(M_N_ratios, meanpswfTime, errorpswfTime, c = "black", fmt='')
	plt.legend(["kb", "kb_interp", "gauss", "pswf"], loc = "upper left")
	plt.xlabel("M/N")
	plt.ylabel("Time, (seconds)")
	plt.xlim(0,2.2)
	plt.savefig("example_Time_plot.png")
	plt.clf()
