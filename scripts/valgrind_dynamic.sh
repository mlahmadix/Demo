#! /bin/sh

valgrind_bin=$(which /usr/bin/valgrind)

if [ "$valgrind_bin" = "" ]
then
	echo "valgrind is not installed"
	echo "please run following command to install it"
	echo "sudo apt-get install valgrind"
	exit 1
fi

# Should be compiled with Debugging symbols (-g)
cmd="valgrind --leak-check=full --leak-resolution=high build/exe/HelloWorld"
nohup $cmd &
bg_pid=$!

sleep 10

kill -2 $bg_pid

wait $bg_pid
