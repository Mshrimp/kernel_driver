#!/bin/sh

if [ -z $1 ]
then
	CHIP=h3
else
	CHIP=$1
fi

make clean && make chip=$CHIP && make app chip=$CHIP && make install chip=$CHIP && make clean

