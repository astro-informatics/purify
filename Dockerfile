FROM ubuntu:18.10

USER root
WORKDIR /build

# Install.
RUN \
  sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list && \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y build-essential && \
  apt-get install -y software-properties-common && \
  apt-get install -y byobu curl git htop man unzip vim wget cmake  g++ zsh && \
  apt-get install -y libopenmpi3 libtiff5-dev libgomp1 libfftw3-dev libboost-all-dev libeigen3-dev && \
  DEBIAN_FRONTEND=noninteractive apt-get install -y casacore-dev && \
  rm -rf /var/lib/apt/lists/*

ENV LD_LIBRARY_PATH /usr/local/lib

# Build sopt
RUN git clone https://github.com/astro-informatics/sopt.git
RUN mkdir -p /build/sopt/build && cd /build/sopt/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
    -Dtests=ON \
    -Dexamples=OFF \
    -Dbenchmarks=OFF \
    -DMPIEXEC:FILEPATH=$(which mpirun) -DMPIEXEC_MAX_NUMPROCS=4 \
    -DMPIEXEC_PREFLAGS="--oversubscribe" && \
    make && make install

# Build purify
RUN git clone https://github.com/astro-informatics/purify.git
RUN mkdir -p /build/purify/build && cd /build/purify/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
    -Dtests=ON \
    -Dexamples=OFF \
    -Ddocasa=ON \
    -Dbenchmarks=OFF \
    -DSopt_GIT_TAG=development \
    -DCFitsIO_URL=https://github.com/UCL-RITS/BinaryBlobs-dependencies/raw/master/Astronomy/cfitsio3410.tar.gz \
    -DMPIEXEC:FILEPATH=$(which mpirun) -DMPIEXEC_MAX_NUMPROCS=4 \
    -DMPIEXEC_PREFLAGS="--oversubscribe" && \
    make && make install

USER purifier
ENV HOME /home/purifier
WORKDIR /mydata
CMD ["zsh"]