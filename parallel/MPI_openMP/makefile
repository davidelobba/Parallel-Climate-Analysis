CC = mpicc
CFLAGS = -std=c99 -Wall -g -fopenmp
COMPFLAGS = -I /apps/netCDF4.7.0--gcc-9.1.0/include -I../utils
LINKFLAGS = -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf

all: parallel
parallel: ../../utils/utils.c parallel.c
	$(CC) $(CFLAGS) -o $@ $^ $(LINKFLAGS) $(COMPFLAGS) -lm
clean:
	rm -r parallel