import os 
import multiprocessing
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt


"""
Script that runs tests of purify on real data.
"""

nice_value = 0

home = os.path.expanduser("~")

outpath = home + "/Purify_RealData/"
data = home + "/dev/purify/data/"

atca = "atca/"
vla = "vla/"
names = [atca + "0332-391", atca + "0114-476", vla + "CygA-X", vla + "3C129BC"]



def make_path(name, outpath, epsilon_factor):
	return outpath + name + "/" + str(epsilon_factor) + "/"

def make_directory(path):
	if not os.path.exists(path):
		os.makedirs(path)

def run_test((name, inpath, size, epsilon_factor)):
	path = make_path(name, outpath, epsilon_factor)
	
	make_directory(path)
	proc = subprocess.Popen(["nice -" + str(nice_value) + " screen -S "+ name.split("/")[-1] + "_" + str(epsilon_factor) + 
		" -d -m ~/dev/purify/build/cpp/example/purify_main" +
		" --name " + path + name.split("/")[-1] +
		" --measurement_set " + inpath + ".ms --n_mean " + str(epsilon_factor) + 
		" --noise " + inpath + ".ms "+
		" --size " + str(size) +
		" --update --diagnostic power_iterations 200"], shell=True)
	print "Waiting for " + path
	while not os.path.exists(path + name.split("/")[-1] + "_solution_whiten_final.fits"):
		time.sleep(10)
	return

params = []
for n in names:
	print "Looking for " + data + n
	if os.path.exists(data + n + ".ms"):
		print n + " found!"
		image_size = 512
		if n == atca + "0114-476":
			image_size = 1024
		params.append((n, data + n, image_size, 1))
		params.append((n, data + n, image_size, 1.2))
		params.append((n, data + n, image_size, 1.3))
		params.append((n, data + n, image_size, 1.4))
		params.append((n, data + n, image_size, 1.5))
	else:
		print n + " not found!"

n_processors = multiprocessing.cpu_count() + 1
p = multiprocessing.Pool(min(n_processors, 20))
p.map(run_test, params)
