#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

void main(void){
	int dev;

	struct msg_st user_str;

	dev = open("/dev/ch3_dev", O_RDWR);
	
	ioctl(dev, CH3_IOCTL_READ, &user_str);

	printf("%s\n", user_str.str);
	
	close(dev);
}
