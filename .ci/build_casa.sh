#!/bin/bash -xe

cwd=$(pwd)
cd /tmp

echo "Building casa"
curl -OL https://github.com/casacore/casacore/archive/v2.4.1.tar.gz
tar zxvf v2.4.1.tar.gz
cd casacore-2.4.1

echo "Building pre-casa stuff"
curl -O https://raw.githubusercontent.com/casacore/casacore/master/.travis/before_script.sh
bash /tmp/before_script.sh

cd /tmp/casacore-2.4.1/build
cmake .. -DUSE_FFTW3=ON -DUSE_OPENMP=ON \
      -DBUILD_PYTHON=OFF -DUSE_THREADS=ON
make
sudo make install

cd $cwd
