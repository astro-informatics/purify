#!/bin/bash -xe

cwd=$(pwd)
cd $HOME

if [ ! -d $HOME/spack ]
   then
       git clone https://github.com/spack/spack.git
fi

. spack/share/spack/setup-env.sh

spack install gcc@7
spack install openmpi@3.0.0
spack install fftw
spack install boost

cd $cwd
