#!/usr/bin/python

import os
import matplotlib.pyplot as plt 
import numpy as np 
from multiprocessing import Pool


def run_test(kernel, oversample, M_N_ratio, i):
	print os.getcwd()
	os.system("../build/cpp/example/sdmm_m31_simulation "+kernel+" "+str(oversample)+" "+str(M_N_ratio)+" "+ str(i))

def kb(x): run_test("kb", 2, 0.3, x)
def kb_interp(x): run_test("kb_interp", 1.375, 0.3, x)
def gauss(x): run_test("gauss", 1.375, 0.3, x)

if __name__ == '__main__':
    p = Pool(4)
    p.map(kb, range(1, 48))


