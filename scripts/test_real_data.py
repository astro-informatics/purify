import os 
import multiprocessing
import time
import numpy as np 
import pyfits
import glob
import subprocess
import matplotlib.pyplot as plt

home = os.path.expanduser("~")

outpath = home + "/Purify_RealData/"
data = home + "/dev/purify/data/"

atca = "atca/"
vla = "vla/"
names = [atca + "0332-391"]
ra_values = []
dec_values = []
params = []

def make_path(name, outpath, epsilon_factor):
	return outpath + name + "/" + str(epsilon_factor) + "/"

def make_directory(path):
	if not os.path.exists(path):
		os.makedirs(path)

def run_test((name, inpath, size, epsilon_factor, ra, dec)):
	path = make_path(name, outpath, epsilon_factor)
	print path
	make_directory(path)
	os.system("screen -S "+ name.split("/")[-1] + "_" + str(epsilon_factor) + " -d -m ~/dev/purify/build/cpp/example/purify_main" +
		" --name " + path + name.split("/")[-1] +
		" --vis " + inpath + "I.vis --n_mean " + str(epsilon_factor) + 
		" --size " + str(size) + " --ra " + str(ra) + " --dec " + str(dec) +
		" --update --diagnostic"
		)
	return

for n in names:
	image_size = 512
	params.append((n, data + n, image_size, 1))
	params.append((n, data + n, image_size, 1.2))
	params.append((n, data + n, image_size, 1.3))
	params.append((n, data + n, image_size, 1.4))
	params.append((n, data + n, image_size, 1.5))

n_processors = multiprocessing.cpu_count() + 1
p = multiprocessing.Pool(min(n_processors, 5))
p.map(run_test, params)
