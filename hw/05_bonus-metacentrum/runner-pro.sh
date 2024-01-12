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
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-128.txt

# Copy the results file to our home.
cp results-pro-scalability-128.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=256
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-256.txt

# Copy the results file to our home.
cp results-pro-scalability-256.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=800
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-800.txt

# Copy the results file to our home.
cp results-pro-scalability-800.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=1600
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-1600.txt

# Copy the results file to our home.
cp results-pro-scalability-1600.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-3200.txt

# Copy the results file to our home.
cp results-pro-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-3200.txt

# Copy the results file to our home.
cp results-pro-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


# ==============================
# 2. scenario performance
PBS_NUM_PPN=1
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-1.txt

# Copy the results file to our home.
cp results-pro-performance-1.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=2
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-2.txt

# Copy the results file to our home.
cp results-pro-performance-2.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=4
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-4.txt

# Copy the results file to our home.
cp results-pro-performance-4.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=8
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-8.txt

# Copy the results file to our home.
cp results-pro-performance-8.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=16
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-16.txt

# Copy the results file to our home.
cp results-pro-performance-16.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=32
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-32.txt

# Copy the results file to our home.
cp results-pro-performance-32.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=64
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 1000 10 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-64.txt

# Copy the results file to our home.
cp results-pro-performance-64.txt $DATADIR || export CLEAN_SCRATCH=false


# ==============================
# 1. scenario scalability
PBS_NUM_PPN=128
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-128.txt

# Copy the results file to our home.
cp results-pro-scalability-128.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=256
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-256.txt

# Copy the results file to our home.
cp results-pro-scalability-256.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=800
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-800.txt

# Copy the results file to our home.
cp results-pro-scalability-800.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=1600
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-1600.txt

# Copy the results file to our home.
cp results-pro-scalability-1600.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-3200.txt

# Copy the results file to our home.
cp results-pro-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


PBS_NUM_PPN=3200
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-scalability-3200.txt

# Copy the results file to our home.
cp results-pro-scalability-3200.txt $DATADIR || export CLEAN_SCRATCH=false


# ==============================
# 2. scenario performance
PBS_NUM_PPN=1
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-1.txt

# Copy the results file to our home.
cp results-pro-performance-1.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=2
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-2.txt

# Copy the results file to our home.
cp results-pro-performance-2.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=4
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-4.txt

# Copy the results file to our home.
cp results-pro-performance-4.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=8
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-8.txt

# Copy the results file to our home.
cp results-pro-performance-8.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=16
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-16.txt

# Copy the results file to our home.
cp results-pro-performance-16.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=32
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-32.txt

# Copy the results file to our home.
cp results-pro-performance-32.txt $DATADIR || export CLEAN_SCRATCH=false

PBS_NUM_PPN=64
export OMP_NUM_THREADS=$PBS_NUM_PPN

# Start the program and save the stdout to results file along with the computational time.
{ ./InstanceGenerator 10 1000 instance.bin && time ./Storage instance.bin output.bin/; } &> results-pro-performance-64.txt

# Copy the results file to our home.
cp results-pro-performance-64.txt $DATADIR || export CLEAN_SCRATCH=false