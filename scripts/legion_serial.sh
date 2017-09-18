#!/bin/bash -l
#$ -S /bin/bash
#$ -l h_rt=1:0:0
#$ -l mem=1G

# Run jobs on specific nodes reserved only for this job, so that we can compare benchmark results.
# XYZ for more available nodes, U to use the RSDG paid ones
#$ -ac allow=U
#$ -ac exclusive
# Use RSDG paid nodes only
#$ -P RCSoftDev
#$ -l paid=1

# Request TMPDIR space (default is 10 GB)
#$ -l tmpfs=1G

# Select number of threads - max 12 for XYZ nodes, 16 for U nodes
#$ -pe smp 16

# Set the working directory to somewhere in your scratch space.  This is
# a necessary step as compute nodes cannot write to $HOME.
#$ -wd /home/cceaiac/Scratch/purify

# Set the name of the job.
#$ -N purifySBenchmark

# Your work *must* be done in $TMPDIR 
cd $TMPDIR

$HOME/purify/build/cpp/benchmarks/measurement_operator > $HOME/Scratch/purify/serialBenchmark.$JOB_ID.out
