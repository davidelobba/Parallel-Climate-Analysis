#! /bin/bash

#PBS -l select=1:ncpus=1:mem=4gb
#PBS -l walltime=0:05:00
#PBS -q short_cpuQ

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# move to the correct folder
cd HPC_Project/serial

# make file
mpicc -std=c99 -Wall -g -o ./serial.out ./serial.c -I /apps/netCDF4.7.0--gcc-9.1.0/include -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf -lm

# run the binary file
mpiexec -n 1 ./serial.out
# mpiexec -n 1 valgrind --leak-check=yes ./serial # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code
# mpiexec -n 1 valgrind --leak-check=yes --track-origins=yes ./serial # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code