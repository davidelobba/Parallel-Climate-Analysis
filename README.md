# Parallel data analysis using NetCDF data format on structured data
## Project HPC 2022

### Install Docker container
- Download Docker from the official site https://www.docker.com/
- Clone this repository
- Add manually the files .cn for the netcdf data to the repository on your local machine
- Open the terminal and move to the repository
- Run ```docker build . -t netcdf-openmpi```
- Run ```docker run --name netcdf -it -v $(pwd):/home/project/ netcdf-openmpi```
- If you want to delete the image, add the flag ```--rm```
- ```-v``` links your current directory on your local machine with the directory on the container
- Test if you have all ready-to-go by running ```ncdump -h <filename>```
- To exit from the container use ```exit```
- For the next time, you can use the flag ```--detach```
  
### Use Docker container in VSCode
- Install the extension Docker on your local machine from VSCode
- Open the extension, you will see your container
- Right click, start your container and then Attach to VSCode
- This will start an ssh to the container
- Navigate to /home and you will see your directory linked to the container
- :warning: all the changes made on the container of that folder will be saved on your local machine as well
- Install in the container the two extensions of C/C++ (see TUTORIAL compiler MPI)
- Open .vscode > cpp_...json and change ```"compilerPath": "/usr/bin/mpicc"```
- You are now ready to develop with MPI!
- Remember to log out from the container and stop it via terminal or VSCode

### Directory on cluster
- Data netCDF in shares/HPC4DataScience/pta/
  - There will be 2 simlation software data:
    - CMCC
    - MPI-ESM1
  - For every simulation there will be data from 1850 to 2100.
  - 
### Useful links
- NETCDF
  - Official site https://www.unidata.ucar.edu/software/netcdf/
  - Professor's slides 
- Docker
  - Cheatsheet https://dockerlabs.collabnix.com/docker/cheatsheet/
  - Work on a container inside VSCode https://stackoverflow.com/questions/69326427/select-interpreter-of-docker-container-in-the-vscode

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
- Test on fire!