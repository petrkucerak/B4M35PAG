#!/usr/bin/env bash

# Our home directory.
DATADIR="/storage/brno2/home/$LOGNAME/"

# Clean SCRATCH on error or exit
trap 'clean_scratch' TERM EXIT

cp $PBS_O_WORKDIR/InstanceGenerator.cpp $SCRATCHDIR  || exit 1
cp $PBS_O_WORKDIR/Storage.cpp $SCRATCHDIR  || exit 1
cp $PBS_O_WORKDIR/Utils.h $SCRATCHDIR  || exit 1
cd $SCRATCHDIR || exit 2 

# Compilation.
g++ -std=c++11 -pipe -Wall -O3 -o InstanceGenerator InstanceGenerator.cpp Utils.h
g++ -std=c++11 -pipe -Wall -O3 -o Storage  Storage.cpp Utils.h

PBS_NUM_PPN=128

export OMP_NUM_THREADS=$PBS_NUM_PPN #omezi pocet paralelnich procesu na pocet procesoru ziskanych od planovaciho systemu

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 400 400 instance.bin && time ./Storage instance.bin output.bin/; } &> results.txt

# Copy the results file to our home.
cp results.txt $DATADIR || export CLEAN_SCRATCH=false
