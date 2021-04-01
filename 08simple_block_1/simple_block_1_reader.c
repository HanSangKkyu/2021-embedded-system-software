#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "simple_block_1.h"

void main(void){
	int fd, ret, cmd;

	printf("Choose cmd : ");
	scanf("%d", &cmd);

	fd = open("/dev/simple_block_1_dev", O_RDWR);
	
	switch(cmd){
		case 1:
			ret = ioctl(fd, WQ, 0);
			break;
		case 2:
			ret = ioctl(fd, WQ_INTERRUPTIBLE, 0);
			break;
		case 3:
			ret = ioctl(fd, WQ_INTERRUPTIBLE_TIMEOUT, 0); 
			break;
	}

	printf("success to consume %d\n", ret);
	
	close(fd);
}
