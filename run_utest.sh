#!/bin/bash

BUILD_DIR=$(pwd)/build/

if [ -d $BUILD_DIR ]
then
	cd $BUILD_DIR
	ctest
else
	echo "please build project firstly"
fi
