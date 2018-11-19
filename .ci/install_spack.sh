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


while sleep 540 ; do echo "=========== make is taking more than 9m - pinging travis =========="; done & # cfits may take long to download
spack compiler list
spack compiler remove clang@8.0.0-apple
spack compiler list
spack install -y gcc@7.2.0
spack compiler add `spack location -i gcc@7.2.0`
spack install -y openmpi@3.0.0 %gcc@7.2.0
spack install -y fftw %gcc@7.2.0
spack install -y boost %gcc@7.2.0

cd $cwd
