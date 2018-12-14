#!/bin/bash -xe

cwd=$(pwd)
cd /tmp
# Download latest stable from mpich
curl -O http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
tar zxf mpich-3.2.1.tar.gz

cd mpich-3.2.1
./configure --prefix=/usr/local  --disable-fortran  > configure.log 2&>1
make -j CFLAGS="-w"  > make.log 2&>1
sudo make install
cd $cwd
