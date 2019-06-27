#!/bin/bash
SCRIPT_PATH=`dirname "$0"`

MODE=$(echo $1 | tr [A-Z] [a-z]) 

PREC=$(echo $2 | tr [A-Z] [a-z])

THREAD_NUM=$3 

C_MODE="Release"
USE_DOUBLE="OFF"
USE_LOG="OFF"
VERBOSE=0

TOTAL_THREAD_NUM=`getconf _NPROCESSORS_ONLN`
if [ x"$THREAD_NUM" = x ]; then
	((THREAD_NUM=TOTAL_THREAD_NUM/2))
fi

if [ "$MODE" = "debug" ]; then
	C_MODE="Debug"
	VERBOSE=1
	USE_LOG="OFF"
elif [ "$MODE" = "log" ]; then
	C_MODE="Release"
	VERBOSE=1
	USE_LOG="ON"
elif [ "$MODE" = "test" ]; then
	C_MODE="Debug"
	USE_LOG="ON"
	VERBOSE=1
	THREAD_NUM=1
else
	C_MODE="Release"
fi


if [ "$PREC" = "double" ]; then
	USE_DOUBLE="ON"
fi

if [ ! -d $SCRIPT_PATH/build ]; then
	mkdir $SCRIPT_PATH/build
fi


set -x
if [ "$MODE" = "clean" ]; then
	cd $SCRIPT_PATH/build && make clean-all
else
	cd $SCRIPT_PATH/build && cmake -DCMAKE_BUILD_TYPE=$C_MODE -DUSE_DOUBLE=$USE_DOUBLE -DUSE_LOG=$USE_LOG -lpthread .. 2> >(tee bin/error.log) && make -j$THREAD_NUM VERBOSE=$VERBOSE 2> >(tee -a bin/error.log) && cd bin
fi
