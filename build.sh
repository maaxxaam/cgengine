#!/bin/bash
START_DIR=$(pwd)
cd "$(dirname "$0")"
cmake .
make -j 8 
cd $START_DIR
