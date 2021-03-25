make
insmod simple_spin.ko
sh mknod.sh
./simple_spin_writer
./simple_spin_reader

rmmod simple_spin
rm /dev/simple_spin_dev
dmesg
make clean
