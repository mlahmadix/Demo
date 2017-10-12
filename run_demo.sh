#!/bin/bash

vcan_exist=`lsmod |grep -ni vcan`
prog_run=0
PWD=$(pwd)
INSTALL_PATH=$PWD/build/exe/
if [ "$vcan_exist" != "" ]
then
	echo "VCAN module already installed and loaded"
	prog_run=1
else
	modprobe vcan >/dev/null 2>&1
	if [ $? -eq 0 ]
	then
		echo "vcan kernel module installed successfully"
		prog_run=1
	else
		echo "couldn't install module, please check"
		prog_run=2
	fi
fi

echo "check if there's a valid vCan0 interface"
ifconfig vcan0 >/dev/null 2>&1
if [ $? -eq 0 ]
then
	echo "interface vcan0 already exists"
	prog_run=1
else
	echo "creating a new interface vcan0 of type vcan"
	ip link add dev vcan0 type vcan >/dev/null 2>&1
	if [ $? -eq 0 ]
	then
		echo "vcan0 interface created successfully"
		prog_run=1
	else
		echo "Cannot create new interface vcan0 of type vcan"
		prog_run=2
	fi
fi

echo "Checking VCan Network interface Status (Up or Down)"
if [ $prog_run -eq 1 ]
then
	vcan_stat=`cat /sys/class/net/vcan0/operstate`
	if [ "$vcan_stat" != "down" ]
	then
		echo "vcan interface already running"
			prog_run=3
	else
		echo "setting up vcna0 interface"
		ip link set up vcan0 >/dev/null 2>&1
		if [ $? -eq 0 ]
		then
		    prog_run=3
		else
		    echo "couldn't set up vcan0 inf"
		    prog_run=2
		fi
	fi
fi

echo "If all checkings are OK, launch Main App"
if [ $prog_run -eq 3 ]
then
	echo "Launching Main App"
	echo "export needed DLL to be able to launch the Main App"
	echo $PWD
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH
	$INSTALL_PATH/HelloWorld
	exit #?
else
	echo "couldn't launch Main App"
	exit 1
fi
