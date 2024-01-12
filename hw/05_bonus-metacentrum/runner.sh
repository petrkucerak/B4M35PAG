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

# ==============================
# 1. scenario scalability
PBS_NUM_PPN=128
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-128.txt

# Copy the results file to our home.
cp results-scalability-128.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=256
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-256.txt

# Copy the results file to our home.
cp results-scalability-256.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=800
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-800.txt

# Copy the results file to our home.
cp results-scalability-800.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=1600
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-1600.txt

# Copy the results file to our home.
cp results-scalability-1600.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-3200.txt

# Copy the results file to our home.
cp results-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-scalability-3200.txt

# Copy the results file to our home.
cp results-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


# ==============================
# 2. scenario performance
PBS_NUM_PPN=1
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-1.txt

# Copy the results file to our home.
cp results-performance-1.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=2
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-2.txt

# Copy the results file to our home.
cp results-performance-2.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=4
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-4.txt

# Copy the results file to our home.
cp results-performance-4.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=8
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-8.txt

# Copy the results file to our home.
cp results-performance-8.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=16
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-16.txt

# Copy the results file to our home.
cp results-performance-16.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=32
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-32.txt

# Copy the results file to our home.
cp results-performance-32.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=64
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 500 500 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-64.txt

# Copy the results file to our home.
cp results-performance-64.txt $DATADIR || export CLEAN_SCRATCH=false


# ==============================
# 2. scenario performance - small
PBS_NUM_PPN=1
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-1-small.txt

# Copy the results file to our home.
cp results-performance-1-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=2
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-2-small.txt

# Copy the results file to our home.
cp results-performance-2-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=4
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-4-small.txt

# Copy the results file to our home.
cp results-performance-4-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=8
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-8-small.txt

# Copy the results file to our home.
cp results-performance-8-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=16
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-16-small.txt

# Copy the results file to our home.
cp results-performance-16-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=32
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-32-small.txt

# Copy the results file to our home.
cp results-performance-32-small.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=64
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 200 200 instance.bin && time ./Storage instance.bin output.bin/; } &> results-performance-64-small.txt

# Copy the results file to our home.
cp results-performance-64-small.txt $DATADIR || export CLEAN_SCRATCH=false