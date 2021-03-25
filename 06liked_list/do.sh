make
insmod simple_linked_list.ko

rmmod simple_linked_list
dmesg | tail -10
make clean
