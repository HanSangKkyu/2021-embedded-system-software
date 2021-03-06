#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ch3.h"

void main(void){
	int dev;

	struct msg_st user_str = {
		0, "reader: hi, writer!"
	};

	user_str.len = strlen(user_str.str);
	
	printf("Writer: Hello, reader!\n");

	dev = open("/dev/ch3_dev", O_RDWR);
	ioctl(dev, CH3_IOCTL_WRITE, &user_str);


	struct msg_st user_str1 = {
		0, "reader: hi, a;sldkjfaskl;d!"
	};

	user_str1.len = strlen(user_str1.str);
	
	printf("Writer: hi, a;sldkjfaskl;d!\n");

	dev = open("/dev/ch3_dev", O_RDWR);
	ioctl(dev, CH3_IOCTL_WRITE, &user_str1);


	close(dev);
}
