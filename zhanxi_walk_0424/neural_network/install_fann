#!/bin/bash

cd
echo GRABBING FILES FROM GITHUB
git clone https://github.com/libfann/fann.git
cd fann/
echo COMPILING FANN
cmake .
make
make install
echo COMPILING SRC
cd src
make
make install
echo MOVING LIB TO /usr/lib/
cp lib* /usr/lib/
echo COMPILING EXAMPLES
cd ../examples/
make
cd
cp -r fann/examples/ .
echo REMOVING FANN FOLDER
rm -rf fann
