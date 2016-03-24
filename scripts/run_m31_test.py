#!/usr/bin/python

import os 
import multiprocessing
import time
import argparse


""""
This script is for running purify simulations with different gridding kernels, producing different outputs.
The tests will run in parallel.
-LP
"""
parser = argparse.ArgumentParser(description = "Run simulations of M31 for a given kernel.")
parser.add_argument("kernel", help = "Name of kernel (kb, kb_interp, pswf, gauss).", type=str)
parser.add_argument("M_N_ratio", help = "Number of visibilities over number of pxiels (M/N).", type=float)
args = parser.parse_args()

kernel = args.kernel
M_N_ratio = args.M_N_ratio

J = 4
oversample = 2
if kernel == "pswf":
  J = 6
if kernel == "kb_interp":
  oversample = 1.375

def run_test(i):
	print os.getcwd()
	os.system("../build/cpp/example/sdmm_m31_simulation " + kernel + " " 
      + str(oversample) + " " +str(J) + " " + str(M_N_ratio) + " " + str(i))

if __name__ == '__main__':
	# kernels test
    for i in range(1, multiprocessing.cpu_count() + 1):
       time.sleep(0.5)
       p = multiprocessing.Process(target=run_test, args = (i,))
       p.start()


