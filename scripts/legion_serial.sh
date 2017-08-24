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
#$ -N purifySBenchmark

# Your work *must* be done in $TMPDIR 
cd $TMPDIR

$HOME/purify/build/cpp/benchmarks/measurement_operator > $HOME/Scratch/purify/serialBenchmark.$JOB_ID.out
