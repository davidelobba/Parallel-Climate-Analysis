# Parallel data analysis using NetCDF data format on structured data
## Project HPC 2022

### Directory on cluster
- Data netCDF in shares/HPC4DataScience/pta/
  - There will be 2 simlation software data:
    - CMCC
    - MPI-ESM1
  - For every simulation there will be data from 1850 to 2100.

### Useful links
- NETCDF
  - Official site https://www.unidata.ucar.edu/software/netcdf/
  - Professor's slides

### Workflow
1. Understand the problem and analyse the type of data that we are working on (combine data together, parallelize the loading of data)
2. Develop and test on a subset of data inside a Docker env on a local machine to debug and be more efficient without using the cluster
3. Run the project on the cluster with different configuration
4. Create the report and the presentation for the exam

### Implementation
- Serial program:
  1. Read netCDF file;
  2. Get variables IDs and values;
  3. Get the precipitation value for each time step for each point of the grid (lat*lon), then sum it up to obtain the sum over 25 or 50 years;
  4. Get the average precipitations over the years for each point of the grid (average precipitations);
  5. Create the file in which will be written the average precipitation over the years for each point of the grid;
  6. Define the dimensions and the variables of the new netCDF file;
  7. Write the average precipitation for each point;
  8. Close the file for freeing up the memory.

### To Do
- Add performance evaluation
- Add Parallelization
- Add a for cicle for the reading of different files
- Add a better way to handle different .c files, e.g. create a main.c to handle the different .c files using a switch or args, or read the file_names from a txt.