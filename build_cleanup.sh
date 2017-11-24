#!/bin/sh

CMAKE=$(which cmake)
MAKE=$(which make)
MKDIR=$(which mkdir)
SRCDIR=$(pwd)
BUILDIR=$(pwd)/build
RM=$(which rm)
LOGGING="ON"
CANLOG="ON"
NMEAPDBG="OFF"
TARGET="DEBUG"

if [ $# -lt 1 ] 
then
	echo "Help: $0 build|clean|check"
	exit 1
fi

if [ "$1" = "build" ]; then
	echo "0%   ---- Building project will start"
	echo "0%   ---- Creating a Temporary build directory"
	$MKDIR $BUILDIR
	cd $BUILDIR
	echo "alias croot='cd $BUILDIR'" >> ~/.bashrc
    . ~/.bashrc
	echo "25%  ---- configuring project environment"
	$CMAKE -DCMAKE_BUILD_TYPE=$TARGET -DLOGDEBUG=$LOGGING -DCANDATALOGGER=$CANLOG -DNMEAPDEBUG=$NMEAPDBG -G "Eclipse CDT4 - Unix Makefiles" $SRCDIR >/dev/null 2>&1
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

elif [ "$1" = "check" ]; then
	echo "0%   ---- project static code check"
	./static_analyzer.sh
	echo "100% ---- Static code check finished"
	echo "100% ---- Please see the check report"
else
	echo "Please enter an appropriate action"
	echo "Help: $0 build|clean|check"
	exit 1
fi
