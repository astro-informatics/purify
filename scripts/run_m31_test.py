#!/usr/bin/python

import os 
import numpy as np 
import multiprocessing
import time

def run_test(kernel, oversample, M_N_ratio, i):
	print os.getcwd()
	os.system("../build/cpp/example/sdmm_m31_simulation "+kernel+" "+str(oversample)+" "+str(M_N_ratio)+" "+ str(i))

def kb(x): run_test("kb", 2, 0.3, x)
def kb_interp(x): run_test("kb_interp", 1.375, 0.3, x)
def gauss(x): run_test("gauss", 1.375, 0.3, x)

if __name__ == '__main__':
	# kaiser-bessel test
    for i in range(1, multiprocessing.cpu_count() + 1):
       time.sleep(0.5)
       p = multiprocessing.Process(target=kb, args = (i,))
       p.start()
    # kaiser-bessel with linear interpolation and minimal oversampling test
    for i in range(1, multiprocessing.cpu_count() + 1):
       time.sleep(0.5)
       p = multiprocessing.Process(target=kb_interp, args = (i,))
       p.start()
    # gaussian test
    for i in range(1, multiprocessing.cpu_count() + 1):
       time.sleep(0.5)
       p = multiprocessing.Process(target=gauss, args = (i,))
       p.start()


