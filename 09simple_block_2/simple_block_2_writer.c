#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "simple_block_2.h"

void main(void){
	int fd, ret, cmd;
	long value;

	printf("Choose cmd : ");
	scanf("%d", &cmd);
	printf("input value : ");
	scanf("%ld", &value);

	fd = open("/dev/simple_block_2_dev", O_RDWR);
	switch(cmd){
		case 1 :
			ret = ioctl(fd, WQ_WAKE_UP, (unsigned long) value);
			break;
		case 2 :
			ret = ioctl(fd, WQ_WAKE_UP_ALL, (unsigned long) value);
			break;
	}


	printf("success to produce: %d\n", ret);

	close(fd);
}
