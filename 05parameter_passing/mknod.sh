#!/bin/sh

MODULE="simple_param_dev"
MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)

mknod /dev/$MODULE c $MAJOR 0