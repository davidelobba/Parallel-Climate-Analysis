CC = mpicc

CFLAGS = -std=c99 -Wall -g 
 
COMPFLAGS = -I /apps/netCDF4.7.0--gcc-9.1.0/include -I../utils

LINKFLAGS = -L /apps/netCDF4.7.0--gcc-9.1.0/lib -lnetcdf


all: serial

serial: ../utils/utils.c serial.c
	$(CC) $(CFLAGS) -o $@ $^ $(LINKFLAGS) $(COMPFLAGS) -lm

clean:
	rm -r serial