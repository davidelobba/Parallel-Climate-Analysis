# load modules
module load mpich-3.2 hdf5-1.10.5--gcc-9.1.0 netcdf-4.7.0--gcc-9.1.0

# make file
mpicc -std=c99 -Wall -g -o ./serial.out ./serial.c -I /apps/netCDF4.7.0--gcc-9.1.0/include -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf -lm

for RUN in $(seq 1 3)
do 
    # create directory if not exists
    mkdir -p output/run_${RUN}

    #launch qsub command
    qsub -N serial_run${RUN} -o output/run_${RUN}/serial_result.log -e output/run_${RUN}/serial_result_err.log serial.sh
done