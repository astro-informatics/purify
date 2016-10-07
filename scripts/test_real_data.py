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
stokes_v_calculation = False 
outpath = home + "/purify_real_data/"
if stokes_v_calculation:
	outpath = home + "/purify_real_data_v/"

data = home + "/Dropbox/Visibility_Files/"

atca = "atca/"
vla = "vla/"
names = [atca + "0332-391", atca + "0114-476", vla + "CygA-X", vla + "3C129BC"]
imsize = ["1024", "1024", "1024", "1024"]
cellsizes = ["2", "2", "0.5","0.4"]

residual_convergences = ["3700", "-1", "20000", "-1"]

def make_path(name, outpath, epsilon_factor):
	return outpath + name + "/" + str(epsilon_factor) + "/"

def make_directory(path):
	if not os.path.exists(path):
		os.makedirs(path)

def run_test((name, inpath, cellsize, size, epsilon_factor, residual_convergence)):
	path = make_path(name, outpath, epsilon_factor)
	noise = " "
	make_directory(path)
	if stokes_v_calculation == True:
		noise = " --noise " + inpath + ".ms "
        cmd = ["screen -S "+ name.split("/")[-1] + "_" + str(epsilon_factor) + 
		" -d -m ~/dev/purify/build/purify" +
		" --name " + path + name.split("/")[-1] +
		" --measurement_set " + inpath + ".ms --n_mean " + str(epsilon_factor) + 
		noise +
		" --cellsize " + cellsize +
		" --size " + str(size) +
		" --diagnostic --power_iterations 200" +
                " --residual_convergence " + residual_convergence +
                " --relative_variation 0.005" +
                " --adapt_iter 100" +
                " --niters 500" +
                " --relative_gamma_adapt 1e-3"
        ]
	proc = subprocess.Popen(cmd, shell=True)
        print cmd
	print "Waiting for " + path
	while not os.path.exists(path + name.split("/")[-1] + "_solution_whiten_final.fits"):
		time.sleep(10)
	return

params = []
for i in range(len(names)):
	i = 2
	n = names[i]
	print "Looking for " + data + n
	if os.path.exists(data + n + ".ms"):
		print n + " found!"
		image_size = imsize[i] 
		pix_size = cellsizes[i]
		params.append((n, data + n, pix_size, image_size, 7, residual_convergences[i]))
	else:
		print n + " not found!"
	break
n_processors = multiprocessing.cpu_count() + 1
p = multiprocessing.Pool(min(n_processors, 1))
p.map(run_test, params)
