#!/bin/bash -xe

cwd=$(pwd)
cd /tmp
# Download latest stable from openmpi
curl -O https://download.open-mpi.org/release/open-mpi/v3.1/openmpi-3.1.1.tar.bz2
tar jxf openmpi-3.1.1.tar.bz2
#
cd openmpi-3.1.1
./configure --prefix=/usr/local > configure.log  2>&1
sudo make -j all install > make.log  2>&1
cd $cwd
