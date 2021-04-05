make
insmod ku_ipc.ko
sh mknod.sh
./ku_ipc_reader
./ku_ipc_writer


rmmod ku_ipc
rm /dev/ku_ipc_dev
# dmesg
make clean
