#!/bin/sh

MODE=$(echo $1 | tr [A-Z] [a-z]) 

PREC=$(echo $2 | tr [A-Z] [a-z])

THREAD_NUM=$3 

USE_DOUBLE="ON"
USE_LOG="OFF"
C_MODE="Release"

if [ "$MODE" = "debug" ]; then
	C_MODE="Debug"
	USE_LOG="ON"
else
	C_MODE="Release"
fi

if [ "$MODE" = "log" ]; then
	USE_LOG="ON"
fi

if [ "$PREC" = "float" ]; then
	USE_DOUBLE="OFF"
fi

TOTAL_THREAD_NUM=`getconf _NPROCESSORS_ONLN`
if [ x"$THREAD_NUM" = x ]; then
	((THREAD_NUM=THREAD_NUM/2))
fi

set -x
if [ "$MODE" = "clean" ]; then
	cd .. && make clean-all
else
	cd .. && cmake -DCMAKE_BUILD_TYPE=$C_MODE -DUSE_DOUBLE=$USE_DOUBLE -DUSE_LOG=$USE_LOG .. && make -j$THREAD_NUM && cd bin
fi
