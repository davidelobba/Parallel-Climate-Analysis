# env variables used in the script for writing csv file
export THREADS=2
export PROCESSES=16
export NUM_NODES=4
export CPUS_PER_NODE=$((($THREADS*$PROCESSES)/$NUM_NODES))

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# make file
mpicc -std=c99 -Wall -g -fopenmp -o ./parallel.out ./parallel.c -I /apps/netCDF4.7.0--gcc-9.1.0/include -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf -lm

for RUN in $(seq 1 3)
do
    # create directory if not exists
    mkdir -p output/run_${RUN}

    #launch qsub command
    qsub -N opMP_MPI_run${RUN}_pr${PROCESSES}_th${THREADS} -l select=${NUM_NODES}:ncpus=$(($CPUS_PER_NODE)):mem=4gb -o output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}.log -e output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}_err.log parallel.sh
    sleep 5 # ensure that the csv file has no race condition
done