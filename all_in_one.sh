#!/bin/zsh
rm -f ./.DBFILE/*

# Build
mkdir build
cd ./build/
cmake ../Src/
make
cd ..

# Test
./MiniSQL < ./Test/input0.txt > ./Test/output0.txt
./MiniSQL < ./Test/input1.txt > ./Test/output1.txt

# Big-Data Test
# python3 ./Test/Generator.py
