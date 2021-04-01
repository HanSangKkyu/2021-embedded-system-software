#!/bin/sh

MODULE="simple_block_1_dev"
MAJOR=$(awk "\$2==\"$MODULE\" {print \$1}" /proc/devices)
mknod /dev/$MODULE c $MAJOR 0
chmod 777 /dev/$MODULE