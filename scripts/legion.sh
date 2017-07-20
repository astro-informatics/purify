#!/bin/bash -l
#$ -S /bin/bash
#$ -l h_rt=0:10:0
#$ -l mem=1G

# Run jobs on XYZ nodes reserved only for this job, so that we can compare benchmark results.
#$ -ac allow=XYZ
##$ -ac exclusive

# I think this is to make it run now?
#$ -now n

# Request TMPDIR space (default is 10 GB)
#$ -l tmpfs=1G

# Set the working directory to somewhere in your scratch space.  This is
# a necessary step as compute nodes cannot write to $HOME.
#$ -wd /home/cceaiac/Scratch/purify

OPTIND=1
getopts ":p" opt
case $opt in
    p)
# Set the name of the job.
#$ -N purifyPBenchmark
# Select the MPI parallel environment and number of cores.
#$ -pe mpi 2
#Automatically set threads to processes per node: if on X nodes = 12 OMP threads
export OMP_NUM_THREADS=$(ppn)
#Run our MPI job with the default modules. Gerun is a wrapper script for mpirun. 
gerun $HOME/purify/build/cpp/benchmarks/mpi_benchmarks > $HOME/Scratch/purify/purifyPBenchmark.$JOB_ID.out
	;;
    *)
	# Set the name of the job.
	#$ -N purifySBenchmark
	# Your work *must* be done in $TMPDIR 
	cd $TMPDIR
	$HOME/purify/build/cpp/benchmarks/measurement_operator > $HOME/Scratch/purify/purifySBenchmark.$JOB_ID.out
	;;
esac
