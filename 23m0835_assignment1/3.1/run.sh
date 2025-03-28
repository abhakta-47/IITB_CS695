#!/bin/bash

make

insmod get_pgfaults.ko
cat /proc/get_pgfaults && cat /proc/vmstat | grep pgfault

rmmod get_pgfaults