make
insmod simple_block_1.ko
sh mknod.sh
./simple_block_1_reader

# rmmod simple_block_1
# rm /dev/simple_block_1_dev
# dmesg
# make clean
