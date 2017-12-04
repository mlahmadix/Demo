[![Build Status : ](https://api.travis-ci.org/mlahmadix/Demo.svg?branch=master)](https://api.travis-ci.org/mlahmadix/Demo.svg)

to build the whole project, please run :
 ./build_project.sh build
 
 
to clean-up project, please run :
 ./cmake_cleanup.sh clean
 
to run the full demo, please run the following command :
 ./run_demo.sh
 
 Miscallenous Tools:
 1- Static Analysis check using cppcheck, please run :
 ./cmake_cleanup.sh check
 
 2- Dynamic Analysis check using valgrind and memory leak check, please run :
 ./valgring_dynamic.sh
 
 3- Performance analysis and Cachegrind call-graph build and show-up, please run :
 ./cachegrind_calls.sh


In order to be able to use and simulate the SAE-J1939 CAN protocol implemented
in this demo, please refer to the following steps:
1- clone the J1939 Linux kernel module project
     git clone https://github.com/imedardia/j1939_driver
2- build the module:
     make
3- Installing the module:
   sudo -i
   insmod /lib/modules/<kernel-headers-version>/kernel/net/can/can.ko (This step is used to avoir CAN external symbols lookup failure)
   exit
   sudo insmod net/can/can-j1939.ko

4- Modifying can-utils "cansend" tool code source
   git clone https://github.com/linux-can/can-utils.git

5- Apply the patch under can-utils/0001-W-A-Support-of-SAE-J1939-protocol.patch
6- build the whole can-utils tools:
   make
   export PATH=$(pwd):$PATH (this to change the default cansend path to current directory instead of /usr/bin/cansend)

7- Create a new virtual CAN interface vcan1, in order to simulate J1939 send and receive:
sudo -i
insmod /lib/modules/<kernel-headers-version>/kernel/net/can/can-gw.ko
If vcan module is not installed, please run: sudo modprobe vcan

ip link add dev vcan0 type vcan (vcan0 will be used by our Demo executable)
ip link add dev vcan1 type vcan (vcan1 will be used by cansend in order to simulate J1939 SPN parameters)

ip link set up vcan0
ip link set up vcan1

create the virtual gateway between vcan0 and vcan1:
cangw -A -s vcan1 -d vcan0 -e (Sender Canal will be vcan1 and receiver is vcan0)

8- now, you can run the demo in one Terminal window: sudo ./HelloWorld
9- In the second window, you can run the script vcan1_simu.sh

Enjoy
