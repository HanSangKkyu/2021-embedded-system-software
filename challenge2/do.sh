make
gcc -o ch2_app_201611244 ch2_app_201611244.c
insmod ch2_mod_201611244.ko
sh mknod.sh
./ch2_app_201611244

rmmod ch2_mod_201611244
rm /dev/ch2_dev
rm ch2_app_201611244
dmesg | tail --lines=6
make clean
