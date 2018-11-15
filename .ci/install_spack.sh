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

spack install gcc@7
spack install openmpi@3.0.0
spack install fftw
spack install boost

cd $cwd
