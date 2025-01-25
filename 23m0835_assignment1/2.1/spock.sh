#!/bin/bash

# Compile the ioctl driver and user space application
make

# Load the ioctl driver
sudo insmod driver.ko
block_size=${1:-10}

# Run the user space application
echo 
echo -e "\e[34mOutput from user space application:\e[0m"
echo -e "------------------------------------------------------------"
sudo ./program.o $block_size
echo -e "------------------------------------------------------------"
echo

# Unload the ioctl module and remove driver
sudo rmmod driver

# Clean up compiled files
make clean

echo 
echo -e "\e[34mLast few dmesg:\e[0m"
echo -e "------------------------------------------------------------"
sudo dmesg | tail -n $((block_size * 2 + 10))
echo -e "------------------------------------------------------------"
echo