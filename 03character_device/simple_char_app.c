#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

void main(void){
	int dev;

	dev = open("/dev/simple_char_dev", O_RDWR);

	write(dev, "Hello character device driver!", 30);

	close(dev);
}
