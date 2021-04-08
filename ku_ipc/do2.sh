# make
# insmod ku_ipc.ko
# sh mknod.sh
./ku_ipc_reader


rmmod ku_ipc
rm /dev/ku_ipc_dev
make clean