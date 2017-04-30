#!/bin/sh

MODE=$(echo $1 | tr [A-Z] [a-z]) 

PREC=$(echo $2 | tr [A-Z] [a-z])

USE_DOUBLE="ON"

if [ "$MODE" = "debug" ]; then
	MODE="Debug"
else
	MODE="Release"
fi

if [ "$PREC" = "float" ]; then
	USE_DOUBLE="OFF"
fi

THREAD_NUM=`getconf _NPROCESSORS_ONLN`
((THREAD_NUM=THREAD_NUM/2))

set -x
cd .. && cmake -DCMAKE_BUILD_TYPE=$MODE -DUSE_DOUBLE=$USE_DOUBLE .. && make -j$THREAD_NUM && cd bin
