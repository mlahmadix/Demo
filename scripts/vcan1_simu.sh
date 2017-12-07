#!/bin/sh

cansend_bin=$(which cansend)
if [ "$cansend_bin" = "" ]
then
	echo "cansend tool not found"
	echo "please run following command"
	echo "sudo apt-get install can-utils"
	exit 1
fi

# Prompt for a loop count ...
printf "Please enter your loop count: "
read -r LOOP_COUNT
counter=0
while true; do
echo "Engine Speed = 2500 RMP"
cansend vcan1 06F00400#FF.FF.FF.20.4E.FF.FF.FF
echo "Engine Oil Pressure = 244 mBar"
cansend vcan1 06FEEF00#FF.FF.FF.7D.FF.FF.FF.FF
echo "Vehicle Speed = 130Km/h"
cansend vcan1 06FEF100#FF.00.82.FF.FF.FF.FF.FF
echo "Engine Coolant Temperature = -25°T"
cansend vcan1 06FEEE00#0F.FF.FF.FF.FF.FF.FF.FF
echo "Volume Level (50%)"
cansend vcan1 06FEEA00#FF.FF.FF.FF.FF.FF.FF.19
if [ $counter -gt $LOOP_COUNT ]; then
   echo "Counter limit reached, exit script."
   exit 1
else
counter=$(($counter + 1))
fi
sleep 2
done
exit 0
