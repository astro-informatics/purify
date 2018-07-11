#!/bin/bash -xe

cwd=$(pwd)
cd /tmp
# Download latest stable from mpich
curl -O http://www.mpich.org/static/downloads/3.2.1/mpich-3.2.1.tar.gz
tar zxf mpich-3.2.1.tar.gz

cd mpich-3.2.1
./configure --prefix=/usr/local
make
make install
cd $cwd
