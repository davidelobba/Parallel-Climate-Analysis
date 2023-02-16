#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <netcdf.h>

#include "utils.h"

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}
#define PATH_FOLDER "/shares/HPC4DataScience/pta/CMCC-CM2-SR5_historical"
#define FILE_NAME "pr_reduce_25y.nc"

#define NDIMS 3
#define NLAT 192
#define NLON 288
#define NTIME 9125
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


int main(){

    /* netCDF file ID and variable ID */
   int ncid, pr_varid, lat_varid, lon_varid;
   int lon_dimid, lat_dimid, time_dimid;

   float pr_in[NLAT][NLON], pr_out[NLAT][NLON];
   float lats[NLAT], lons[NLON];

   size_t start[NDIMS], count[NDIMS];
   
   int dimids[NDIMS];
   int retval;
   int lat, lon, time = 0;

   const char *file = "pr_day_CMCC-CM2-SR5_historical_r1i1p1f1_gn_18500101-18741231.nc";

   char nc_file[strlen(PATH_FOLDER) + strlen(file) + 2];
   combine(nc_file, PATH_FOLDER, file);
   printf("%s\n", nc_file);


   /* Open the file with read-only access, indicated by NC_NOWRITE flag */
   if ((retval = nc_open(nc_file, NC_NOWRITE, &ncid)))
        ERR(retval);

   if ((retval = nc_inq_varid(ncid, LAT_NAME, &lat_varid)))
      ERR(retval);

   if ((retval = nc_inq_varid(ncid, LON_NAME, &lon_varid)))
      ERR(retval);

   if ((retval = nc_get_var_float(ncid, lat_varid, &lats[0])))
      ERR(retval);

   if ((retval = nc_get_var_float(ncid, lon_varid, &lons[0])))
      ERR(retval);
 
    /*
    Check if lats and lons were read correctly
    for (lat = 0; lat < NLAT; lat++){
        printf("%f \n", lats[lat]);
    }
   for (lon = 0; lon < NLON; lon++){
        printf("%f \n", lons[lon]);
     }
    */
   
   if ((retval = nc_inq_varid(ncid, PR_NAME, &pr_varid)))
      ERR(retval);

   count[0] = 1;
   count[1] = NLAT;
   count[2] = NLON;
   start[1] = 0;
   start[2] = 0;

    /* Get the precipitation value for each time step for each point of the grid, then sum it up to obtain the sum over 25 years */
   for (time = 0; time < NTIME; time++)
   {
        start[0] = time;
        if ((retval = nc_get_vara_float(ncid, pr_varid, start, count, &pr_in[0][0])))
            ERR(retval);

        for(lat = 0; lat < NLAT; lat++)
        {
            for(lon = 0; lon < NLON ; lon++)
            {
                pr_out[lat][lon] = pr_out[lat][lon] + pr_in[lat][lon];
            }
      }
    }

    /* Get the average precipitations over 25 years for each point of the grid (average precipitations) */
    for(lat = 0; lat < NLAT; lat++)
        {
            for(lon = 0; lon < NLON ; lon++)
            {
                pr_out[lat][lon] = (pr_out[lat][lon])/9125;
            }
        }
    
    printf("*** SUCCESS reading file %s!\n", nc_file);

    /* Create the file */
    if ((retval = nc_create(FILE_NAME, NC_CLOBBER, &ncid)))
        ERR(retval);

    if ((retval = nc_def_dim(ncid, LAT_NAME, NLAT, &lat_dimid)))
        ERR(retval);

    if ((retval = nc_def_dim(ncid, LON_NAME, NLON, &lon_dimid)))
        ERR(retval);

    if ((retval = nc_def_dim(ncid, TIME_NAME, NC_UNLIMITED, &time_dimid)))
        ERR(retval);

    if ((retval = nc_def_var(ncid, LAT_NAME, NC_FLOAT, 1, &lat_dimid, &lat_varid)))
        ERR(retval);
    
    if ((retval = nc_def_var(ncid, LON_NAME, NC_FLOAT, 1, &lon_dimid, &lon_varid)))
        ERR(retval);
    
    if ((retval = nc_put_att_text(ncid, lat_varid, UNITS, strlen(DEGREES_NORTH), DEGREES_NORTH)))
        ERR(retval);
    
    if ((retval = nc_put_att_text(ncid, lon_varid, UNITS, strlen(DEGREES_EAST), DEGREES_EAST)))
        ERR(retval);

    dimids[0] = time_dimid;
    dimids[1] = lat_dimid;
    dimids[2] = lon_dimid;

    /* Define the netCDF variables for the precipitation data */
    if ((retval = nc_def_var(ncid, PR_NAME, NC_FLOAT, NDIMS, dimids, &pr_varid)))
        ERR(retval);
    
    /* Assign units attributes to the netCDF variables */
    if ((retval = nc_put_att_text(ncid, pr_varid, UNITS, strlen(PR_UNITS), PR_UNITS)))
        ERR(retval);
    
    /* End define mode */
    if ((retval = nc_enddef(ncid)))
        ERR(retval);

    /* Write the coordinate variable data. This will put the latitudes and longitudes of our data grid into the netCDF file */
    if ((retval = nc_put_var_float(ncid, lat_varid, &lats[0])))
          ERR(retval);
    if ((retval = nc_put_var_float(ncid, lon_varid, &lons[0])))
          ERR(retval);

    count[0] = 1;
    count[1] = NLAT;
    count[2] = NLON;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    /* Write in the new netCDF file the average over the years for each point of the grid of the precipitations */
    if ((retval = nc_put_vara_float(ncid, pr_varid, start, count, &pr_out[0][0])))
        ERR(retval);

    /*Close the file, freeing all resources */
    if ((retval = nc_close(ncid)))
        ERR(retval);       

    printf("*** SUCCESS writing example file %s!\n", FILE_NAME);
    
    return 0;
}
