#!bin/bash

cd prog
make final buildimg
make realclean
# sh build.sh
cd ..
bochs -q
