#!/bin/sh

cansend_bin=$(which cansend)
if [ $cansend_bin = "" ]
then
	echo "cansend tool not found"
	echo "please run following command"
	echo "sudo apt-get install can-utils"
	exit 1
fi
#Engine Speed = 2500 RMP
cansend vcan0 06F00400#FF.FF.FF.20.4E.FF.FF.FF
#Engine Oil Pressure = 144 mBar
cansend vcan0 06FEEF00#FF.FF.FF.7D.FF.FF.FF.FF
#Vehicle Speed = 130Km/h
cansend vcan0 06FEF100#FF.00.82.FF.FF.FF.FF.FF
#Engine Coolant Temperature = -25°T
cansend vcan0 06FEEE00#0F.FF.FF.FF.FF.FF.FF.FF
#Volume Level (50%)
cansend vcan0 06FEEA00#FF.FF.FF.FF.FF.FF.FF.19
exit 0
