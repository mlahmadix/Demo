#!/bin/sh
CPPCHECK=$(which cppcheck)

if [ "$CPPCHECK" = "" ]
then
     echo "cpp check tool is not installed"
     echo "please run:"
     echo "sudo apt-get install cppcheck"
     exit 1
 else
	$CPPCHECK --enable=all --inconclusive .
fi
exit 0
