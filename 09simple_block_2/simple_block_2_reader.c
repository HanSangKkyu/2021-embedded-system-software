#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "simple_block_2.h"

void main(void){
	int fd, ret, cmd;

	printf("[%d] Choose cmd : ", getpid());
	scanf("%d", &cmd);

	fd = open("/dev/simple_block_2_dev", O_RDWR);
	
	switch(cmd){
		case 1:
			ret = ioctl(fd, WQ, 0);
			break;
		case 2:
			ret = ioctl(fd, WQ_EX, 0);
			break;
	}

	printf("success to consume %d\n", ret);
	
	close(fd);
}
