#!/bin/bash -l
# Batch script to run a hybrid parallel job with the upgraded software
# stack under SGE with Intel MPI.
#$ -S /bin/bash
# 1. Request ten minutes of wallclock time (format hours:minutes:seconds).
#$ -l h_rt=3:00:0
# 2. Request 1 gigabyte of RAM (must be an integer)
#$ -l mem=4G
# 3. Request 15 gigabyte of TMPDIR space per node (default is 10 GB)
#$ -l tmpfs=1G
# 4. Set the name of the job.
#$ -N purify_sim
# 5. Select the MPI parallel environment and 24 cores.
#$ -pe mpi 800
# 6. Set the working directory to somewhere in your scratch space.  This is
# a necessary step with the upgraded software stack as compute nodes cannot
# write to $HOME.
#$ -wd /home/ucaslmw/Scratch/
# 7. Automatically set threads to processes per node: if on X nodes = 12 OMP threads
export OMP_NUM_THREADS=$(ppn)
# 7. Run our MPI job with the default modules. Gerun is a wrapper script for mpirun. 
gerun $HOME/Scratch/purify/build/purify $HOME/Scratch/purify/build/sim.yaml > $HOME/Scratch/purify_sim.out
