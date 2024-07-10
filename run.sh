#!/bin/bash
START_DIR=$(pwd)
cd "$(dirname "$0")"
cd bin
if [ -z $2 ]; then
	./$1
else
	gdb ./$1
fi
cd $START_DIR
