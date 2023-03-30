# env variables used in the script for writing csv file
export NUM_NODES=2
export CPUS_PER_NODE=8
export PROCESSES=$(($NUM_NODES*$CPUS_PER_NODE))

# local env variable for path folder creation
RUN=1

# create directory if not exists
mkdir -p output/run_${RUN}

#launch qsub command
qsub -N MPI_run${RUN}_pr${PROCESSES} -l select=${NUM_NODES}:ncpus=$(($CPUS_PER_NODE)):mem=4gb -o output/run_${RUN}/MPI_result_processes${PROCESSES}.log -e output/run_${RUN}/MPI_result_processes${PROCESSES}_err.log parallel.sh