import os 
import multiprocessing
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt

def run_test((i, kernel, M_N_ratio, start_time)):
	time.sleep(start_time)
	J = 4
	oversample = 2
	if kernel == "pswf":
  		J = 6
	if kernel == "kb_interp":
  		oversample = 1.375

  	
  	os.system("../build/cpp/example/sdmm_ms_clean_m31_simulation " + kernel + " " 
      + str(oversample) + " " +str(J) + " " +str(M_N_ratio) + " " + str(i))
  	
  	results_file = "../build/outputs/M31_results_" + kernel + "_" + str(i) + ".txt"
  	results = np.loadtxt(results_file, dtype = str)
  	SNR = results[0]
  	total_time = results[1]
 
	os.system("rm " + results_file)
  	return [float(SNR), float(total_time)]


if __name__ == '__main__':
	M_N_ratios = np.arange(1, 11) * 0.2
	args = []
	n_tests = 5
	test_num = 0
	kernels = ["kb", "kb_interp", "pswf", "gauss"]
	total_tests = n_tests * len(kernels) * len(M_N_ratios)
	for i in range(1, n_tests + 1):
		for k in kernels:
			for m in M_N_ratios:
				test_num = test_num + 1
				args.append((test_num, k, m, test_num * 1./ total_tests * 30.))
				print test_num
	n_processors = multiprocessing.cpu_count() + 1
	p = multiprocessing.Pool(min(n_processors, 41)) # Limiting the number of processes used to 40, otherwise it will cause problems with the user limit
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
	
	tableSNR = np.array([M_N_ratios, meankbSNR, errorkbSNR, meankb_interpSNR, 
		                                 errorkb_interpSNR, meangaussSNR, errorgaussSNR, meanpswfSNR, errorpswfSNR])
	tableTime = np.array([M_N_ratios, meankbTime, errorkbTime, meankb_interpTime, 
		                                 errorkb_interpTime, meangaussTime, errorgaussTime, meanpswfTime, errorpswfTime])
	np.savetxt('sdmm_ms_clean_SNRtable', tableSNR, delimiter=',')
	np.savetxt('sdmm_ms_clean_Timetable', tableTime, delimiter=',')
	plt.errorbar(M_N_ratios, meankbSNR, errorkbSNR, fmt='')
	plt.errorbar(M_N_ratios, meankb_interpSNR, errorkb_interpSNR, c = "red", fmt='')
	plt.errorbar(M_N_ratios, meangaussSNR, errorgaussSNR, c = "green", fmt='')
	plt.errorbar(M_N_ratios, meanpswfSNR, errorpswfSNR, c = "black", fmt='')
	plt.legend(["Kaiser-Bessel", "Kaiser-Bessel Linear Interpolation", "Gaussian", "PSWF"], loc = "upper left")
	plt.xlabel("M/N")
	plt.ylabel("SNR, db")
	plt.xlim(0, 2.2)
	plt.savefig("sdmm_ms_clean_SNR_plot.pdf")
	plt.clf()

	plt.errorbar(M_N_ratios, meankbTime, errorkbTime, fmt='')
	plt.errorbar(M_N_ratios, meankb_interpTime, errorkb_interpTime, c = "red", fmt='')
	plt.errorbar(M_N_ratios, meangaussTime, errorgaussTime, c = "green", fmt='')
	plt.errorbar(M_N_ratios, meanpswfTime, errorpswfTime, c = "black", fmt='')
	plt.legend(["Kaiser-Bessel", "Kaiser-Bessel Linear Interpolation", "Gaussian", "PSWF"], loc = "upper left")
	plt.xlabel("M/N")
	plt.ylabel("Time, (seconds)")
	plt.xlim(0, 2.2)
	plt.savefig("sdmm_ms_clean_Time_plot.pdf")
	plt.clf()
