#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "simple_spin.h"

void main(void){
	int dev;

	struct str_st user_str = {
		0, "reader: hi, writer!"
	};

	user_str.len = strlen(user_str.str);
	
	printf("Writer: Hello, reader!\n");

	dev = open("/dev/simple_spin_dev", O_RDWR);
	ioctl(dev, SIMPLE_SPIN_WRITE, &user_str);

	close(dev);
}
