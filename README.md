# Parallel climate analysis - Structured data
## Project HPC 2022

<!-- TABLE OF CONTENTS -->
<details open>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#introduction">Introduction</a></li>
    <li><a href="How-to-run-the-code-man_technologist">How to run the code</a></li>
      <ul>
        <li><a href="#serial-code-bug">Serial code</a></li>
        <li><a href="#mpi-code-rocket">MPI code</a></li>
        <li><a href="#hybrid-code-rocket">Hybrid code</a></li>
      </ul>
    </li>
    <li><a href="#dataset">Dataset</a></li>
    <li><a href="#serial-implementation">Serial Implementation</a></li>
    <li><a href="#parallel-implementation">Parallel Implementation</a>
      <ul>
        <li><a href="#mpi">MPI</a></li>
        <li><a href="#hybrid-parallelization">Hybrid Parallelization</a></li>
      </ul>
    </li>
    <li><a href="#performance-and-benchmarking">Performance and Benchmarking</a></li>
      <ul>
        <li><a href="#mpi-performance">MPI performance</a></li>
        <li><a href="#hybrid-parallelization-performance">Hybrid Parallelization performance</a></li>
      </ul>
    <li><a href="#conclusions">Conclusions</a></li>
    <li><a href="#contacts">Contacts</a></li>
  </ol>
</details>

### Introduction

Our work wants to explore the implementation of a parallel climate analysis tool using structured data to exploit the hardware resources of the [Unitn HPC cluster](https://sites.google.com/unitn.it/hpc/). In order to extract and analyze vast amounts of climate data, it is important to exploit parallel computing to achieve in an efficient way and in short time the desired results.

### How to run the code :man_technologist:
- Clone the repository

   ```sh
   git clone https://github.com/davidelobba/Parallel-Climate-Analysis.git
   ```
- cd to the desired implementation the user wants to run

   ```sh
   cd serial/
   ```
   or
   ```sh
   cd parallel/MPI_openMP/
   ```
### Serial code :bug:
Run from command line the following code:
  ```sh
   bash launch.sh
   ```
In this way, the serial code is going to be run. Specifically the code will be run on 0.25, 0.5 and 1.0 of problem size, which are the sizes of the dataset. It is important to note that each configuration is going to be run 3 times, in order to avoid I/O unpredictable behaviours.

### MPI code :rocket:

Before launching the code, it is important to edit in the [launch.sh](https://github.com/davidelobba/Parallel-Climate-Analysis/tree/main/parallel/MPI_openMP/launch.sh) the threads part setting just 1 thread, otherwise the MPI with OpenMP solution will be run.

Then, run from command line the following code:
  ```sh
   bash launch.sh
   ```
The code will be run on 0.25, 0.5 and 1.0 of problem size, which are the sizes of the dataset. Moreover, the code will be run with different numbers of processes, in order to have a consistent benchmark. It is important to note that each configuration is going to be run 3 times, in order to avoid I/O unpredictable behaviours.

### Hybrid code :rocket:
If the user has previously edited the [launch.sh](https://github.com/davidelobba/Parallel-Climate-Analysis/tree/main/parallel/MPI_openMP/launch.sh), go back to the default settings for the thread variable, otherwise the MPI solution will be run.

Then, run from command line the following code:
  ```sh
   bash launch.sh
   ```
The code will be run on 0.25, 0.5 and 1.0 of problem size, which are the sizes of the dataset. Moreover, the code will be run with different numbers of processes and threads, in order to have a consistent benchmark. It is important to note that each configuration is going to be run 3 times, in order to avoid I/O unpredictable behaviours.


### Dataset
For this project, we decided to use precipitation flux datasets available in a shared folder of the HPC@Unitrento cluster. The data are stored in a NetCDF (Network Common Data Form) format. The data are organized in the following way:
- Dimensions: are used to define the size and shape of the data arrays in a NetCDF file. Organized by time, which for a single file is based on 25 years (9125 days, or called records), latitude and longitude, which are, respectively, based on 192 and 288 points.
- Variables: are used to store the actual data in a NetCDF file. Organized by time, latitude, longitude and precipitation flux. Specifically, the variable that we analyse is the precipitation flux

In addition, in order to obtain more consistent results, given the fact that a single NetCDF file was made by only 25 years of recording, we decided to augment the dataset exploiting the CDO library. In the end, we obtained a dataset of 165 years of recording.

### Serial Implementation
For the serial implementation, we decided to adopt the official NetCDF library to read and write data. The main steps are the following:
- Reading of the NetCDF data
- Reduction operation over the time variable
- Writing of the new NetCDF file, which contains only the latitude and longitude variables with the precipitation flux average value of every point of the grid

### Parallel Implementation
Regarding the parallel program, we decided to implement two strategie to parallelize the serial code:
- MPI
- Hybrid implementation, MPI with OpenMP

### MPI
For the MPI parallelization, we decided to parallelize our program along the time dimension. For every MPI process, we split the time in a uniform way across the available processes, managing the non-zero remainder division by giving the remaining records to one process. In this way, every process has a subset of data to read and sum on a local matrix. Then we performed the reduction and we obtained the final matrix in which for every point of the grid we had the sum over the entire time. In the end, we divided every point of the output matrix by the number of records/days and stored the new data in a NetCDF file on the cluster. These last instructions were very fast to compute, thus we have not parallelized them.

### Hybrid Parallelization
To have a further analysis, we added to the MPI solution the OpenMP standard API. Specifically, we followed the same procedure as the MPI version with just a few key differences to allow multithreading. To add multithreading capabilities, we worked on the nested loop, after the collection part of a precipitation flux of a specific record, by dividing the grids into sub-grids which depend on the number of threads available.


### Performance and Benchmarking
For the evaluation of the performances, we used the following metrics:

$$ Speedup = {{Time_{Serial}}\over {Time_{Parallel(p)}}} $$

$$ Efficiency = {{Speedup} \over {p}} = {{Time_{Serial}}\over {p \times Time_{Parallel(p)}}} $$

where $p$ is the number of processes used to run the parallel code.

In order to obtain a benchmark which was not subjected to race conditions or job overlapping, we submitted each job to the cluster separately.
Moreover, we decided to run each configuration 3 times to have more consistent benchmarks. We did this because we noticed that sometimes the I/O operations were impacting a lot the efficiency of our program. We decided to test the problem with different sizes: 0.25 corresponds to a quarter of the total number of records/days, 0.5 to half records and finally 1.0 corresponds to the original problem, the totality of dataset’s records.

### MPI performance
We tried different settings for the PBS and we ended up evaluating the algorithm using the place=scatter:excl and fixing the number of nodes to 4. For requesting the number of cores, we adopted the following relation:

$$ Cores\ per\ node = {{Processes} \over {Nodes}} $$

As expected, the MPI version outperforms the serial one.

For a deeper analysis we suggest to look at the section 5.2 of the [project's report](https://github.com/davidelobba/Parallel-Climate-Analysis/tree/main/HPC_report.pdf) 

### Hybrid parallelization performance
For the hybrid parallelization we decided to fix the number of nodes to 4 and use a placement strategy place=scatter:excl. This type of configuration helped us to compare the two parallel versions. For the assignment of the right number of cores to run our parallel program, we adopted the following relation:

$$ Cores\ per\ node = {{Threads \times Processes} \over {Nodes}} $$

Threads are allocated on the same node and, if possible, on the same socket to guarantee fast communication between them. Also the hybrid parallelization outperforms the serial solution.

For a deeper analysis we suggest to look at the section 5.3 of the [project's report](https://github.com/davidelobba/Parallel-Climate-Analysis/tree/main/HPC_report.pdf) 

### Conclusions
With this project, we designed, implemented and tested three different solutions to perform climate analysis using structured data on precipitation flux records. As expected, parallel versions outperform the serial one. In addition, it can be argued that simply increasing the number of processors does not always lead to improved performances. Rather, it is important to balance the allocation of resources, while also accounting for any potential overheads introduced.

### Contacts
Francesco Laiti - [Github](https://github.com/laitifranz/) - [Linkedin](https://www.linkedin.com/in/francesco-laiti/) - [UniTN email](mailto:francesco.laiti@studenti.unitn.it) | Davide Lobba - [Github](https://github.com/davidelobba/) - [Linkedin](https://www.linkedin.com/in/davide-lobba) - [UniTN email](mailto:davide.lobba@studenti.unitn.it)
