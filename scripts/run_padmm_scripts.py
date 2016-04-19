import subprocess
import glob

scripts = glob.glob("padmm_plot_*.py")
for s in scripts:
	args = ["python", s]
	print "Running " + s + " script."
	p = subprocess.Popen(args)
	p.wait()