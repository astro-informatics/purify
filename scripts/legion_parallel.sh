#!/bin/bash -l
#$ -S /bin/bash
#$ -l h_rt=12:0:0
#$ -l mem=1G

# Run jobs on XYZ nodes reserved only for this job, so that we can compare benchmark results.
#$ -ac allow=XYZ
#$ -ac exclusive

# Request TMPDIR space (default is 10 GB)
#$ -l tmpfs=1G

# Set the working directory to somewhere in your scratch space.  This is
# a necessary step as compute nodes cannot write to $HOME.
#$ -wd /home/cceaiac/Scratch/purify

# Set the name of the job.
#$ -N purifyPBenchmark

# Select the MPI parallel environment and number of cores. Has to be multiple of 12 for XYZ nodes
#$ -pe mpi 12

#Automatically set threads to processes per node: eg. if on X nodes = 12 OMP threads
export OMP_NUM_THREADS=$(ppn)

#Run our MPI job with the default modules. Gerun is a wrapper script for mpirun. 
gerun $HOME/purify/build/cpp/benchmarks/mpi_benchmarks > $HOME/Scratch/purify/parallelBenchmark.01.$JOB_ID.out
