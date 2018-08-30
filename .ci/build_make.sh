#!/bin/bash -xe

cwd=$(pwd)
cd /tmp

wget http://ftp.gnu.org/gnu/make/make-4.1.tar.gz
tar xvf make-4.1.tar.gz
cd make-4.1/
./configure --prefix=/usr/
make
sudo make install

cd $cwd
