# local env variable for path folder creation
RUN=1

# create directory if not exists
mkdir -p output/run_${RUN}

#launch qsub command
qsub -N serial_run${RUN} -o output/run_${RUN}/serial_result.log -e output/run_${RUN}/serial_result_err.log serial.sh