#!/bin/bash -xe

cwd=$(pwd)
cd /tmp
curl -O https://raw.githubusercontent.com/casacore/casacore/master/.travis/before_script.sh

echo "Building pre-casa stuff"
bash /tmp/before_script.sh

echo "Building casa"
curl -OL https://github.com/casacore/casacore/archive/v2.4.1.tar.gz
tar zxvf v2.4.1.tar.gz
cd casacore-2.4.1
mkdir build
cd build
cmake .. -DUSE_FFTW3=ON -DUSE_OPENMP=ON \
      -DBUILD_PYTHON=OFF -DUSE_THREADS=ON
make
make install

cd $cwd
