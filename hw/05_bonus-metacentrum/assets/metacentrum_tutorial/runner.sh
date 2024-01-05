#!/usr/bin/env bash

# Our home directory.
DATADIR="/storage/brno2/home/$LOGNAME/"

# Clean SCRATCH on error or exit
trap 'clean_scratch' TERM EXIT

cp $PBS_O_WORKDIR/main.cpp $SCRATCHDIR  || exit 1
cd $SCRATCHDIR || exit 2 

# Compilation.
g++ -std=c++11 -O2 -o HelloWorld main.cpp

# Start the program and save the stdout to results file along with the computational time.
{ time ./HelloWorld; } &> results.txt

# Copy the results file to our home.
cp results.txt $DATADIR || export CLEAN_SCRATCH=false