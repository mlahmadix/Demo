#!/bin/sh
cachegrind_bin=$(which /usr/bin/kcachegrind)

if [ $cachegrind_bin = "" ]
then
	echo "kcachegrind is not installed"
	echo "please run following command to install it"
	echo "sudo apt-get install kcachegrind"
	exit 1
fi

# Should be compiled with Debugging symbols (-g)
cmd="valgrind --tool=callgrind --demangle=yes --callgrind-out-file=hello_world.callgrind build/exe/HelloWorld"
nohup $cmd &
bg_pid=$!

sleep 10

kill -2 $bg_pid

wait $bg_pid

# open profile.callgrind with kcachegrind
# i- You can analyze the call-graph
# ii- You can analyze memory and cpu usage by each thread
# iii- You can even see machine code and so many other features
sudo $cachegrind_bin hello_world.callgrind


