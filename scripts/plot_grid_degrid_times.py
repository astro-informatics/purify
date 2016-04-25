import os 
import multiprocessing
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt

def run_test((i, kernel, number_of_samples, start_time)):
	time.sleep(start_time)
	J = 4
	oversample = 2
	if kernel == "pswf":
  		J = 6
	if kernel == "kb_interp":
  		oversample = 1.375

  	
  	os.system("../build/cpp/example/time_gridding_degridding " + kernel + " " 
      + str(oversample) + " " +str(J) + " " +str(i) + " " + str(number_of_samples))
  	
  	results_file = "../build/outputs/Grid_Degrid_timing_" + kernel + "_" + str(i) + ".txt"
  	results = np.loadtxt(results_file, dtype = str)
  	grid_time = results[0]
  	grid_time_error = results[1]
  	degrid_time = results[2]
  	degrid_time_error = results[3]
 
	os.system("rm " + results_file)
  	return [float(grid_time), float(grid_time_error), float(degrid_time), float(degrid_time_error)]


if __name__ == '__main__':
	M_N_ratios = np.arange(1, 11)
	args = []
	test_num = 0
	kernels = ["kb", "kb_interp", "pswf", "gauss"]
	total_tests = len(kernels) * len(M_N_ratios)
	for k in kernels:
		for m in M_N_ratios:
			test_num = test_num + 1
			args.append((m, k, 100, test_num * 1./ total_tests * 30.))
			print test_num
	n_processors = multiprocessing.cpu_count() + 1
	p = multiprocessing.Pool(min(n_processors, 41)) # Limiting the number of processes used to 40, otherwise it will cause problems with the user limit
	results = p.map(run_test, args)

	gridkbtime = []
	gridkbtime_error = []
	degridkbtime = []
	degridkbtime_error = []

	gridkb_interptime = []
	gridkb_interptime_error = []
	degridkb_interptime = []
	degridkb_interptime_error = []

	gridpswftime = []
	gridpswftime_error = []
	degridpswftime = []
	degridpswftime_error = []

	gridgausstime = []
	gridgausstime_error = []
	degridgausstime = []
	degridgausstime_error = []	
	for m in M_N_ratios:
		for i in range(len(args)):
			if m == args[i][1]:
				if args[i][1] == "kb":
					gridkbtime.append(results[i][0])
					gridkbtime_error.append(results[i][1])
					degridkbtime.append(results[i][2])
					degridkbtime_error.append(results[i][3])
				if args[i][1] == "kb_interp":
					gridkb_interptime.append(results[i][0])
					gridkb_interptime_error.append(results[i][1])
					degridkb_interptime.append(results[i][2])
					degridkb_interptime_error.append(results[i][3])
				if args[i][1] == "gauss":
					gridpswftime.append(results[i][0])
					gridpswftime_error.append(results[i][1])
					degridpswftime.append(results[i][2])
					degridpswftime_error.append(results[i][3])
				if args[i][1] == "pswf":
					gridgausstime.append(results[i][0])
					gridgausstime_error.append(results[i][1])
					degridgausstime.append(results[i][2])
					degridgausstime_error.append(results[i][3])
	


	plt.errorbar(M_N_ratios/10. * 2, gridkbtime, gridkbtime_error, fmt='')
	plt.errorbar(M_N_ratios/10. * 2, gridkb_interptime, gridkb_interptime_error, c = "red", fmt='')
	plt.errorbar(M_N_ratios/10. * 2, gridpswftime, gridpswftime_error, c = "green", fmt='')
	plt.errorbar(M_N_ratios/10. * 2, gridgausstime, gridgausstime_error, c = "black", fmt='')
	plt.legend(["Kaiser-Bessel (KB)", "KB Linear Interpolation", "Gaussian", "PSWF"])
	plt.xlabel("M/N")
	plt.ylabel("Time (seconds)")
	plt.xlim(0, 2.2)
	plt.savefig("Grid_Time_plot.pdf")

	plt.clf()

	plt.errorbar(M_N_ratios/10. * 2, degridkbtime, degridkbtime_error, fmt='')
	plt.errorbar(M_N_ratios/10. * 2, degridkb_interptime, degridkb_interptime_error, c = "red", fmt='')
	plt.errorbar(M_N_ratios/10. * 2, degridpswftime, degridpswftime_error, c = "green", fmt='')
	plt.errorbar(M_N_ratios/10. * 2, degridgausstime, degridgausstime_error, c = "black", fmt='')
	#plt.legend(["Kaiser-Bessel (KB)", "KB Linear Interpolation", "Gaussian", "PSWF"])
	plt.xlabel("M/N")
	plt.ylabel("Time (seconds)")
	plt.xlim(0, 2.2)
	plt.savefig("Degrid_Time_plot.pdf")
	plt.clf()
