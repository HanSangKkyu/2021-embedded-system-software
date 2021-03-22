make
insmod simple_param.ko my id="-200"
sh mknod.sh
./simple_param_app

rmmod simple_param
rm /dev/simple_param_dev
dmesg | tail --lines=6
make clean
