FROM ubuntu:focal-20221019

RUN apt-get update

# https://stackoverflow.com/questions/53079135/how-can-i-pass-arguments-or-bypass-it-in-docker-build-process
## for apt to be noninteractive
ENV DEBIAN_FRONTEND noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN true

## preesed tzdata, update package index, upgrade packages and install needed software
RUN echo "tzdata tzdata/Areas select Europe" > /tmp/preseed.txt; \
    echo "tzdata tzdata/Zones/Europe select Rome" >> /tmp/preseed.txt; \
    debconf-set-selections /tmp/preseed.txt && \
    apt-get update && \
    apt-get install -y tzdata

# install OpenMPI
RUN apt-get -y install openmpi-bin 

# install NetCDF
RUN apt-get -y install netcdf-bin 

WORKDIR /home