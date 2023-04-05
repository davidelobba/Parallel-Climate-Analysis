# env variables used in the script for writing csv file
export THREADS=2
export PROCESSES=16
export NUM_NODES=4
export CPUS_PER_NODE=$((($THREADS*$PROCESSES)/$NUM_NODES))
export PLACE=scatter

# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# make file
mpicc -std=c99 -Wall -g -fopenmp -o ./parallel.out ./parallel.c -I /apps/netCDF4.7.0--gcc-9.1.0/include -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf -lm

for RUN in $(seq 1 3)
do
    for PLACE in scatter pack scatter:excl pack:excl
    do
        for NUM_NODES in 1 2
        do
            for PROCESSES in 2 4 8 16
            do
                for THREADS in 2 4 8 16
                do
                    # create directory if not exists
                    mkdir -p output/run_${RUN}
    
                    export CPUS_PER_NODE=$((($THREADS*$PROCESSES)/$NUM_NODES))
    
                    #launch qsub command
                    qsub -N opMP_MPI_run${RUN}_pr${PROCESSES}_th${THREADS} -l select=${NUM_NODES}:ncpus=$(($CPUS_PER_NODE)):mem=4gb -l place=${PLACE} -o output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}.log -e output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}_err.log parallel.sh
                    sleep 25 # ensure that the csv file has no race condition
                done
            done
        done
    done
done