// Code adapted from 
//   https://docs.unidata.ucar.edu/netcdf-c/current/pres__temp__4D__rd_8c_source.html for reading task
//   https://docs.unidata.ucar.edu/netcdf-c/current/pres__temp__4D__wr_8c.html for writing task

#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>

#ifdef _OPENMP
    #include <omp.h> //version 201107 -> 3.1
#endif

// #include "utils.h"

/*Change input netCDF path here*/
#define INPUT_FILE "/shares/HPC4DataScience/pta/CMCC-CM2-SR5_historical/pr_day_CMCC-CM2-SR5_historical_r1i1p1f1_gn_20000101-20141231.nc" // 20000101-20141231 18500101-18741231
/*Define output name file. Saved in the same directory of serial.c*/
#define OUTPUT_FILE "pr_reduce.nc"

#define NLAT 192
#define NLON 288
#define START_LAT -90.0
#define START_LON 0.0
#define LAT_NAME "lat"
#define LON_NAME "lon"
#define TIME_NAME "time"
#define PR_NAME "pr"
#define DEGREES_EAST "degrees_east"
#define DEGREES_NORTH "degrees_north"
#define LAT_UNITS "degrees_north"
#define LON_UNITS "degrees_east"
#define UNITS "units"
#define PR_UNITS "kg m-2 s-1"

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

int main(int argc, char* argv[]){

    #ifdef _OPENMP
    printf("OpenMP version: %d\n", _OPENMP);
    #else
    printf("No OpenMP support\n");
    #endif

    MPI_Init(NULL, NULL);

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    /*DEBUG*/
    if (world_rank == 0) printf("--- INFO: World size = %d ---\n", world_size);

    double start_time, end_time;
    int retval, thread_count;

    /* netCDF file ID and variable ID */
    int ncid;
    int pr_varid, lat_varid, lon_varid;
    int lon_dimid, lat_dimid, time_dimid;
    int ndims;
    size_t nrecord;

    /* GET INFO STEP*/
    /* Open the file with read-only access, indicated by NC_NOWRITE flag */
    start_time = MPI_Wtime();

    if ((retval = nc_open(INPUT_FILE, NC_NOWRITE, &ncid))) ERR(retval);
    /* Retrieve dimensions info */
    if ((retval = nc_inq_ndims(ncid, &ndims))) ERR(retval);

    if ((retval = nc_inq_dimid(ncid, LAT_NAME, &lat_dimid))) ERR(retval);
    if ((retval = nc_inq_dimid(ncid, LON_NAME, &lon_dimid))) ERR(retval);
    if ((retval = nc_inq_dimid(ncid, TIME_NAME, &time_dimid))) ERR(retval);

    if ((retval = nc_inq_dimlen(ncid, time_dimid, &nrecord))) ERR(retval);

    /*DEBUG: Check dim and time dimension
    printf("--- INFO: found dim = %d and nrecord = %zu ---\n", ndims, nrecord);
    */

    ndims -= 1; // we do not use the bnds dimension, so we need to reduce by one the total number of dims

    /* Retrieve variables info */
    if ((retval = nc_inq_varid(ncid, LAT_NAME, &lat_varid))) ERR(retval);
    if ((retval = nc_inq_varid(ncid, LON_NAME, &lon_varid))) ERR(retval);
    if ((retval = nc_inq_varid(ncid, PR_NAME, &pr_varid))) ERR(retval);

    float lats[NLAT], lons[NLON];
    if ((retval = nc_get_var_float(ncid, lat_varid, &lats[0]))) ERR(retval);  /* Read the coordinate variable data. */
    if ((retval = nc_get_var_float(ncid, lon_varid, &lons[0]))) ERR(retval);
    
    end_time = MPI_Wtime();

    /* DEBUG: time for execution*/
    printf("## Process %d, Time GET INFO STEP: %f seconds ##\n", world_rank, end_time - start_time);

    /* DEBUG: Check if lats and lons were read correctly
    for (int lat = 0; lat < nlat; lat++)
        printf("%f \n", lats[lat]);
    for (int lon = 0; lon < nlon; lon++)
        printf("%f \n", lons[lon]);
    */

    /* READING STEP*/
    start_time = MPI_Wtime();

    float local_pr_in[NLAT][NLON], total_pr_out[NLAT][NLON], local_pr_out[NLAT][NLON];
    size_t start[] = {0, 0, 0}; // 3-dim array
    size_t count[] = {1, NLAT, NLON}; // 3-dim array 

    const int rec_per_process = nrecord/world_size;
    int local_start_record = world_rank * rec_per_process;
    int local_end_record = (world_rank + 1) * rec_per_process;
    int lat, lon;

    if(argc > 0) thread_count = strtol(argv[1], NULL, 10);

    // #ifdef _OPENMP
    //     int thread_rank = omp_get_thread_num();
    //     int thread_count = omp_get_num_threads();
    // #else
    //     int thread_rank = 0;
    //     int thread_count = 1;
    // #endif

    if (world_rank == world_size - 1) local_end_record = nrecord; // manage the last batch of record that may not be divided properly

    //OMP on NLAT NLON
    // /* Get the precipitation value for each time step for each point of the grid, then sum it up to obtain the sum over the years */
    // for (int rec = local_start_record; rec < local_end_record; rec++){
    //     start[0] = rec;
    //     if ((retval = nc_get_vara_float(ncid, pr_varid, start, count, &local_pr_in[0][0]))) ERR(retval);

    //     //start_time = omp_get_wtime();
    //     #pragma omp parallel num_threads(thread_count) default(none) private(lat, lon) shared(local_pr_out, local_pr_in)
    //         // collapse(2) reference: https://stackoverflow.com/questions/13357065/how-does-openmp-handle-nested-loops // in omp for the index are default private, but for coherence we explicitly declare it
    //         #pragma omp for collapse(2)
    //             for(lat = 0; lat < NLAT; lat++){
    //                 for(lon = 0; lon < NLON; lon++){
    //                     local_pr_out[lat][lon] = local_pr_out[lat][lon] + local_pr_in[lat][lon];
    //                     //printf("# of rank thread: %d", omp_get_thread_num());
    //                 }
    //         }
    //     //#pragma omp  barrier //Implicit barrier at: end of parallel do/for, single
    // }

    //OMP on NTIME
    /* Get the precipitation value for each time step for each point of the grid, then sum it up to obtain the sum over the years */
    int rec;
    #pragma omp parallel for num_threads(thread_count) \
            default(shared) private(rec, lat, lon, local_pr_in) reduction(+:local_pr_out) schedule(auto) // default(none) private(rec, lat, lon, local_pr_in) shared(local_pr_out) \ reduction(+:local_pr_out) schedule(auto)
    for (rec = local_start_record; rec < local_end_record; rec++){
        start[0] = rec;
        //if ((retval = nc_get_vara_float(ncid, pr_varid, start, count, &local_pr_in[0][0]))) ERR(retval);
        nc_get_vara_float(ncid, pr_varid, start, count, &local_pr_in[0][0]);
        for(lat = 0; lat < NLAT; lat++){
            for(lon = 0; lon < NLON; lon++){
                local_pr_out[lat][lon] = local_pr_out[lat][lon] + local_pr_in[lat][lon];
                //printf("# of rank thread: %d", omp_get_thread_num());
            }
        }
        //#pragma omp  barrier //Implicit barrier at: end of parallel do/for, single
    }

    end_time = MPI_Wtime();
    /* DEBUG: time for execution*/
    printf("## Process %d, Time READING STEP 1: %f seconds ##\n", world_rank, end_time - start_time);
    printf("--- WR: %d pr_out_local[0][0] = %f\n", world_rank, local_pr_out[0][0]);

    MPI_Reduce(&local_pr_out, &total_pr_out, NLAT*NLON, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD); // The resulting summation happens on a per-element basis of the matrix

    MPI_Barrier(MPI_COMM_WORLD); // only for benchmarking

    if (world_rank == 0){
        start_time = MPI_Wtime();

        /*DEBUG*/
        printf("--- WR: %d pr_out_globa[0][0] = %f\n", world_rank, total_pr_out[0][0]);
        
        /* Get the average precipitations over the years for each point of the grid (average precipitations) */
        for(int lat = 0; lat < NLAT; lat++){
            for(int lon = 0; lon < NLON ; lon++){
                total_pr_out[lat][lon] /= nrecord;
            }
        }
    
        end_time = MPI_Wtime();

        /* DEBUG: time for execution*/
        printf("## Time READING STEP 2: %f seconds ##\n", end_time - start_time);

        /* Close the file */
        if ((retval = nc_close(ncid))) ERR(retval);

        //printf("--- SUCCESS reading data from file %s ---\n", INPUT_FILE);


        /* WRITING STEP: only done by process 0*/
        start_time = MPI_Wtime();
        /* Create the file */
        if ((retval = nc_create(OUTPUT_FILE, NC_CLOBBER, &ncid))) ERR(retval);

        if ((retval = nc_def_dim(ncid, LAT_NAME, NLAT, &lat_dimid))) ERR(retval);
        if ((retval = nc_def_dim(ncid, LON_NAME, NLON, &lon_dimid))) ERR(retval);
        if ((retval = nc_def_dim(ncid, TIME_NAME, 1, &time_dimid))) ERR(retval); // from NC_UNLIMITED to 1 because we reduce the time

        if ((retval = nc_def_var(ncid, LAT_NAME, NC_FLOAT, 1, &lat_dimid, &lat_varid))) ERR(retval);
        if ((retval = nc_def_var(ncid, LON_NAME, NC_FLOAT, 1, &lon_dimid, &lon_varid))) ERR(retval);
        int dimids[] = {time_dimid, lat_dimid, lon_dimid}; // 3-dim array
        if ((retval = nc_def_var(ncid, PR_NAME, NC_FLOAT, ndims, dimids, &pr_varid))) ERR(retval); /* Define the netCDF variables for the precipitation data */

        if ((retval = nc_put_att_text(ncid, lat_varid, UNITS, strlen(DEGREES_NORTH), DEGREES_NORTH))) ERR(retval);
        if ((retval = nc_put_att_text(ncid, lon_varid, UNITS, strlen(DEGREES_EAST), DEGREES_EAST))) ERR(retval);
        if ((retval = nc_put_att_text(ncid, pr_varid, UNITS, strlen(PR_UNITS), PR_UNITS))) ERR(retval);     /* Assign units attributes to the netCDF variables */

        if ((retval = nc_enddef(ncid))) ERR(retval); /* End define mode */

        /* Write the coordinate variable data. This will put the latitudes and longitudes of our data grid into the netCDF file */
        if ((retval = nc_put_var_float(ncid, lat_varid, &lats[0]))) ERR(retval);
        if ((retval = nc_put_var_float(ncid, lon_varid, &lons[0]))) ERR(retval);

        count[0] = 1; count[1] = NLAT; count[2] = NLON;
        start[0] = 0; start[1] = 0; start[2] = 0;

        /* Write in the new netCDF file the average over the years for each point of the grid of the precipitations */
        if ((retval = nc_put_vara_float(ncid, pr_varid, start, count, &total_pr_out[0][0]))) ERR(retval);

        end_time = MPI_Wtime();

        /* DEBUG: time for execution*/
        printf("## Time WRITING STEP: %f seconds ##\n", end_time - start_time);

        /*Close the file, freeing all resources */
        if ((retval = nc_close(ncid))) ERR(retval);       

        printf("--- SUCCESS writing data on file %s ---\n", OUTPUT_FILE);
    }

    MPI_Finalize();
    return 0;
}