# env variables used in the script for writing csv file
export THREADS=1
export NUM_NODES=4
export PLACE=scatter:excl
export PROBLEM_SIZE
export PROCESSES
export CPUS_PER_NODE

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# make file
mpicc -std=c99 -Wall -g -fopenmp -o ./parallel.out ./parallel.c -I /apps/netCDF4.7.0--gcc-9.1.0/include -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf -lm

for PROBLEM_SIZE in 0.25 0.5 1
do
    for PROCESSES in 4 8 16 24 32 64 96 128
    do
        for RUN in $(seq 1 3)
        do
            mkdir -p output/log/mpi_only/problem_${PROBLEM_SIZE}/np_${PROCESSES}/run_${RUN}  # create directory if not exists
            export CPUS_PER_NODE=$((($PROCESSES)/$NUM_NODES))
            qsub -Wblock=true -N parallel_run${RUN} -l select=${NUM_NODES}:ncpus=$(($CPUS_PER_NODE)):mem=4gb -l place=${PLACE} -o output/log/mpi_only/problem_${PROBLEM_SIZE}/np_${PROCESSES}/run_${RUN}/result.log -e output/log/mpi_only/problem_${PROBLEM_SIZE}/np_${PROCESSES}/run_${RUN}/error.log parallel.sh
        done
    done
done