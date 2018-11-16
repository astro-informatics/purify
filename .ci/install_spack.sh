#!/bin/bash -xe

cwd=$(pwd)
cd $HOME

# Are we using the cache directory or it's empty?
if [ ! -f $HOME/spack/README.md ]
then
    rm -rf spack
    git clone https://github.com/spack/spack.git
fi

. spack/share/spack/setup-env.sh

spack install -y gcc@7.2.0
spack install -v -y openmpi@3.0.0 %gcc
spack install -v -y fftw %gcc
spack install -v -y boost %gcc

cd $cwd
