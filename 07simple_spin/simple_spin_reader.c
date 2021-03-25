#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "simple_spin.h"

void main(void){
	int dev;

	struct str_st user_str;

	dev = open("/dev/simple_spin_dev", O_RDWR);
	
	ioctl(dev, SIMPLE_SPIN_READ, &user_str);

	printf("%s\n", user_str.str);
	
	close(dev);
}
