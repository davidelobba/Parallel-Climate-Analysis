#! /bin/bash

#parameters
#PBS -l select=1:ncpus=1:mem=1gb

# set max exec time
#PBS -l walltime=0:05:00

# set queue
#PBS -q short_cpuQ

# load modules
module load openmpi-4.0.4 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0 mpich-3.2

# move to the correct folder
cd HPC_Project/parallel/MPI_openMP

# make file
make

# run the binary file
mpiexec -n 1 ./parallel 4
# mpiexec -n 4 valgrind --leak-check=yes ./parallel # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code
# mpiexec -n 1 valgrind --leak-check=yes --track-origins=yes ./parallel # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code