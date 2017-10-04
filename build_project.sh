#!/bin/sh

CMAKE=$(which cmake)
MAKE=$(which make)
LOGGING="OFF"
FILETRC="OFF"

echo "0%   ---- configuring project environment"
$CMAKE -DLOGDEBUG=$LOGGING -DFILECREAT=$FILETRC . >/dev/null 2>&1
if [ $? -eq 0 ]
then
	sleep 1
	echo "50%  ---- building project"
	$MAKE  >/dev/null 2>&1
	if [ $? -eq 0 ]
	then
		sleep 1
		$MAKE install >/dev/null 2>&1
		if [ $? -eq 0 ]
		then
			sleep 1
			echo "100% ---- installation successfull"
			exit 0
		else
			sleep 1
			echo "100% ---- Error installing binary and dependencies"
		fi
	else
		sleep 1
		echo "100% ---- Failed Building project"
		echo "100% ---- Please fix all errors"
		exit 2
	fi
else
	sleep 1
	echo "100% ---- Failed preparing building environment"
	echo "100% ---- please check all project dependencies"
	exit 1
fi
