make
insmod ch3.ko
sh mknod.sh
./ch3_mod_201611244_writer
./ch3_mod_201611244_reader

rmmod ch3
rm /dev/ch3_dev
dmesg
make clean
