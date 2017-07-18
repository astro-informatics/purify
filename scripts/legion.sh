#!/bin/bash -l
#$ -S /bin/bash
#$ -P RCSoftDev
#$ -l paid=1
#$ -l h_rt=0:10:0
#$ -l mem=1G

# Request TMPDIR space (default is 10 GB)
#$ -l tmpfs=10G

# Set the name of the job.
#$ -N serialBenchmark

# Set the working directory to somewhere in your scratch space.  This is
# a necessary step as compute nodes cannot write to $HOME.
#$ -wd /home/cceaiac/Scratch/purify

# Your work *must* be done in $TMPDIR 
cd $TMPDIR

$HOME/purify/build/cpp/benchmarks/measurement_operator > $HOME/Scratch/purify/results_$JOB_ID.out
