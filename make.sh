#!/bin/bash
SCRIPT_PATH=`dirname "$0"`

MODE=$(echo $1 | tr [A-Z] [a-z]) 

THREAD_NUM=$3 

C_MODE="test"
VERBOSE=0

TOTAL_THREAD_NUM=`getconf _NPROCESSORS_ONLN`
if [ x"$THREAD_NUM" = x ]; then
	((THREAD_NUM=TOTAL_THREAD_NUM/2))
fi

if [ "$MODE" = "test" ]; then
	USE_LOG="ON"
	VERBOSE=1
	THREAD_NUM=1
fi


if [ ! -d $SCRIPT_PATH/build ]; then
	mkdir $SCRIPT_PATH/build
fi


set -x
if [ "$MODE" = "clean" ]; then
	cd $SCRIPT_PATH/build && make clean-all
else
	cd $SCRIPT_PATH/build && make -j$THREAD_NUM VERBOSE=$VERBOSE 2> >(tee bin/error.err) && cd bin
fi
