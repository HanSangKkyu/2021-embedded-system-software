make
insmod ch3_mod_201611244.ko
sh mknod.sh
./ch3_mod_201611244_writer
./ch3_mod_201611244_reader

rmmod ch3_mod_201611244
rm /dev/ch3_dev
dmesg | tail -10
make clean
