#!/bin/bash

# build
make

# Load the kernel module
sudo insmod get_memstats.ko

# Function to get memory statistics using Linux tools
get_memory_stats() {
    local pid=$1
    local metrics_file="/proc/$pid/status"
    echo "Metrics accroding to  $metrics_file"
    echo 'Virtual mem:'; cat "$metrics_file" | grep 'VmSize'
    echo 'Physical mem:'; cat "$metrics_file" | grep 'VmRSS'
}

# Set the PID in the kernel module
set_pid() {
    local pid=$1
    echo $pid | sudo tee /sys/kernel/mem_stats/pid
    echo 'K' | sudo tee /sys/kernel/mem_stats/unit
}

# Get the values from the kernel module
get_kernel_values() {
    local pid=$(cat /sys/kernel/mem_stats/pid)
    local virtmem=$(cat /sys/kernel/mem_stats/virtmem)
    local physmem=$(cat /sys/kernel/mem_stats/physmem)
    local unit=$(cat /sys/kernel/mem_stats/unit)
    echo "Kernel Module Values:"
    echo "PID: $pid"
    echo "Virtual Memory: $virtmem $unit"
    echo "Physical Memory: $physmem $unit"
}

# Main script
if [ $# -ne 1 ]; then
    echo "Usage: $0 <pid>"
    exit 1
fi

PID=$1

# Get memory stats using Linux tools
get_memory_stats $PID

# Set the PID in the kernel module
set_pid $PID

# Get the values from the kernel module
get_kernel_values

# Unload the kernel module
sudo rmmod get_memstats