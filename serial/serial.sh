#! /bin/bash

#parameters
#PBS -l select=2:ncpus=2:mem=1gb

# set max exec time
#PBS -l walltime=0:05:00

# set queue
#PBS -q short_cpuQ

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# make file
make

# script to run
mpiexec -n 1 ./serial