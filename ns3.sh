#!/bin/bash


# build and test ns3
hg clone http://code.nsnam.org/ns-3-allinone
cd ns-3-allinone
./download.py -n ns-3-dev
./build.py --enable-examples --enable-tests
cd ns-3-dev
./test.py
./waf --run hello-simulator

# copy files from repository
cp ../../*.h scratch/
cp ../../*.cc scratch/
cp ../../*.txt scratch/

# generate folder for logfiles
mkdir logfiles

# run streaming example
./waf --run "myLena --delay=100 --datarate=1000 --pathlogfiles=logfiles/ --pathframelist=scratch/framelist.txt"