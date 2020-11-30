#! /bin/bash

for i in `seq 0 99`;
do
    cd /home/zwhuang/opt/testfor1/$i
    for j in `seq 0 499`;
    do
    g++ $j.cpp -o testfor$j
    done
done
