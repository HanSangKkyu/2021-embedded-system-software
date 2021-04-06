#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "ku_ipc_lib.c"
#include "ku_ipc.h"

void main(void){
	// int fd, ret, cmd;

	// printf("[%d] Choose cmd : ", getpid());
	// scanf("%d", &cmd);

	// fd = open("/dev/simple_block_2_dev", O_RDWR);
	
	// switch(cmd){
	// 	case 1:
	// 		ret = ioctl(fd, WQ, 0);
	// 		break;
	// 	case 2:
	// 		ret = ioctl(fd, WQ_EX, 0);
	// 		break;
	// }

	// printf("success to consume %d\n", ret);
	
	// close(fd);
	// ku_msgget(0, KU_IPC_CREAT);
	// printf("ku_ipc_reader: %d\n",KU_IPC_CREAT);
	// printf("ku_ipc_reader: %d\n", ku_msgget(0, KU_IPC_CREAT));
	// printf("ku_ipc_reader: %d\n", ku_msgget(0, KU_IPC_EXCL));
	struct msgbuf my_msgbuf = {0, "a"};
	printf("ku_ipc_reader: %d\n", ku_msgsnd(0, &my_msgbuf, 1, KU_IPC_NOWAIT));
	printf("ku_ipc_reader: %d\n", ku_msgsnd(0, &my_msgbuf, 1, KU_IPC_NOWAIT));
	printf("ku_ipc_reader: %d\n", ku_msgsnd(0, &my_msgbuf, 1, KU_IPC_NOWAIT));
	printf("ku_ipc_reader: %d\n", ku_msgsnd(0, &my_msgbuf, 1, KU_IPC_NOWAIT));
}
