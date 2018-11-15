
export SPACK_ROOT=$HOME/spack
. $SPACK_ROOT/share/spack/setup-env.sh

spack load gcc@7
spack load openmpi@3.0.0
spack load fftw
spack load boost
