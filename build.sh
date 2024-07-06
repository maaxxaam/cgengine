#!/bin/bash
START_DIR=$(pwd)
cd "$(dirname "$0")"
cmake .
make
cd $START_DIR
