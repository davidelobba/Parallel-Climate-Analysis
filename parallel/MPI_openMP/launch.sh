# env variables used in the script for writing csv file
export THREADS=2
export PROCESSES=16
export NUM_NODES=4
export CPUS_PER_NODE=(($THREADS*$PROCESSES)/$NUM_NODES))

# local env variable for path folder creation
RUN=1

# create directory if not exists
mkdir -p output/run_${RUN}

#launch qsub command
qsub -N opMP_MPI_run${RUN}_pr${PROCESSES}_th${THREADS} -l select=${NUM_NODES}:ncpus=$(($CPUS_PER_NODE)):mem=4gb -o output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}.log -e output/run_${RUN}/openMP_MPI_result_processes${PROCESSES}_threads${THREADS}_err.log parallel.sh