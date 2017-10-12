#!/bin/sh

CMAKE=$(which cmake)
MAKE=$(which make)
MKDIR=$(which mkdir)
SRCDIR=$(pwd)
BUILDIR=$(pwd)/build
RM=$(which rm)
LOGGING="ON"
FILETRC="OFF"

if [ $# -lt 1 ] 
then
	echo "Help: $0 build|clean"
	exit 1
fi

if [ "$1" = "build" ]; then
	echo "0%   ---- Building project will start"
	echo "0%   ---- Creating a Temporary build directory"
	$MKDIR $BUILDIR
	cd $BUILDIR

	echo "25%   ---- configuring project environment"
	$CMAKE -DLOGDEBUG=$LOGGING -DFILECREAT=$FILETRC $SRCDIR >/dev/null 2>&1
	if [ $? -eq 0 ]
	then
		echo "50%  ---- building project"
		$MAKE >/dev/null 2>./make.log
		if [ $? -eq 0 ]
		then
			$MAKE install >/dev/null 2>&1
			if [ $? -eq 0 ]
			then
				echo "100% ---- installation successfull"
				exit 0
			else
				echo "100% ---- Error installing binary and dependencies"
			fi
		else
			echo "100% ---- Failed Building project"
			echo "100% ---- Please see make.log"
			exit 2
		fi
	else
		echo "100% ---- Failed preparing building environment"
		echo "100% ---- please check all project dependencies"
		exit 1
	fi

elif [ "$1" = "clean" ]; then
	echo "0%   ---- project build cleanup"
	$RM -rf $BUILDIR
	echo "100% ---- All project cleanup done"
	
else
	echo "Please enter an appropriate action"
	echo "Help: $0 build|clean"
	exit 1
fi
