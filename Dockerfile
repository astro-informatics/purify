FROM uclrits/sopt:18.10

USER root
WORKDIR /build
ENV LD_LIBRARY_PATH /usr/local/lib

RUN docker-apt-install casacore-dev

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
#docker build -t uclrits/purify . # to build from the current Dockerfile
#docker run -it --name sopt -v /full/localpath:/mydata  uclrits/sopt
#docker run -it --name purify -v $(pwd)/data:/mydata uclrits/purify
