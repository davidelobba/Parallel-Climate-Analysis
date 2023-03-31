#! /bin/bash

#PBS -l walltime=0:05:00
#PBS -q short_cpuQ
#PBS -v NUM_NODES,CPUS_PER_NODE,PROCESSES

echo number of processes ${PROCESSES}
echo number of cores per node ${CPUS_PER_NODE}
echo number of nodes ${NUM_NODES}
echo ""

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# move to the correct folder
cd HPC_Project/parallel/MPI

# run the binary file
mpiexec -n ${PROCESSES} ./parallel.out
# mpiexec -n 1 valgrind --leak-check=yes ./parallel # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code
# mpiexec -n 1 valgrind --leak-check=yes --track-origins=yes ./parallel # https://stackoverflow.com/questions/34851643/using-valgrind-to-spot-error-in-mpi-code