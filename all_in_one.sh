#!/bin/zsh
rm *.table
rm *.index
mkdir build
cd ./build/
cmake ../Src/
make
cd ..
# for i in $(seq 1 10)
# do
#     ./build/Generator $i `expr 10000 \* $i` 100
# done
./MiniSQL < ./Test/input.txt > ./Test/output.txt
cat ./Test/output.txt
