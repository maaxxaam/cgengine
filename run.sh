#!/bin/bash
START_DIR=$(pwd)
cd "$(dirname "$0")"
cd bin
if [ -z $2 ]; then
	cage -- ./$1
else
	gdb -ex "set follow-fork-mode child" --args cage -- ./$1
fi
cd $START_DIR
