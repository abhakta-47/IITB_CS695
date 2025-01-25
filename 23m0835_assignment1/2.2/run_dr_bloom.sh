#!/bin/bash

function echo_green() {
    echo -e "\e[32m$1\e[0m"
}

function echo_red() {
    echo -e "\e[31m$1\e[0m"
}

## Building the applications
make
if [ $? -ne 0 ]; then
    echo_red "Make failed"
    exit $?
fi
echo_green "Make sucessfull !!"

## Build your ioctl driver and load it here
sudo rmmod driver2.ko
sudo insmod driver2.ko
if [ $? -ne 0 ]; then
    echo_red "insmod failed"
    exit $?
fi
echo_green "insmod successfull !!"

###############################################

# Launching the control station
./control_station 13 &
c_pid=$!
echo "Control station PID: $c_pid"

sleep 3;

# Launching the soldiers
./soldier $c_pid 1 &
s_pid1=$!
echo "Soldier PID (exits): $s_pid1"

./soldier $c_pid 1  &
s_pid2=$!
echo "Soldier PID (hangs): $s_pid2"

./soldier $c_pid 1  &
s_pid3=$!
echo "Soldier PID (hangs): $s_pid3"

./soldier $c_pid 1  &
s_pid4=$!
echo "Soldier PID (exits): $s_pid4"

wait $c_pid

kill -9 $s_pid1
kill -9 $s_pid2
kill -9 $s_pid3
kill -9 $s_pid4

## Remove the driver here
sudo rmmod driver2.ko


sleep 2
echo_green "to view the relevant dmesg run:"
echo_green "sudo dmesg | grep '[DRIVER2.2]'"
