#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ku_ipc_lib.c"
#include "ku_ipc.h"

void main(void){
	// int fd, ret, cmd;
	// long value;

	// printf("Choose cmd : ");
	// scanf("%d", &cmd);
	// printf("input value : ");
	// scanf("%ld", &value);

	// fd = open("/dev/ku_ipc_dev", O_RDWR);
	// switch(cmd){
	// 	case 1 :
	// 		ret = ioctl(fd, WQ_WAKE_UP, (unsigned long) value);
	// 		break;
	// 	case 2 :
	// 		ret = ioctl(fd, WQ_WAKE_UP_ALL, (unsigned long) value);
	// 		break;
	// }


	// printf("success to produce: %d\n", ret);

	// close(fd);
	// ku_msgclose(0);
	// printf("ku_ipc_writer: %d\n", ku_msgclose(0));
	// printf("ku_ipc_writer: %d\n", ku_msgclose(1));
	struct msgbuf my_msgbuf = {0, "a"};
	// printf("ku_ipc_reader: %d\n", ku_msgsnd(0, &my_msgbuf, 1, KU_IPC_NOWAIT));
	// ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg)
	printf("ku_ipc_writer: %d\n", ku_msgrcv(0, &my_msgbuf, 4, 0, KU_IPC_NOWAIT));
}
