#!/bin/bash

echo_green() {
    echo -e "\e[32m$1\e[0m"
}
echo_yellow() {
    echo -e "\e[33m$1\e[0m"
}
echo_red() {
    echo -e "\e[31m$1\e[0m"
}

# Functions to laod and unload a module
load_module() {
    local module_name=$1
    echo_green "Loading module $module_name"
    sudo insmod $module_name.ko
}
unload_module() {
    local module_name=$1
    echo_red "Unloading module $module_name"
    sudo rmmod $module_name
}

make all

# task 1.1
echo_yellow "Runing task 1.1"
load_module lkm1
unload_module lkm1

# task 1.2
echo_yellow "Runing task 1.2"

load_module lkm1
unload_module lkm1