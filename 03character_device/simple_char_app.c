#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>

void main(void){
	int dev;
	char stst;
	dev = open("/dev/simple_char_dev", O_RDWR);

	write(dev, "Hello character device driver!", 30);
	read(dev, &stst, 30);
	printf("%c\n", stst);

	close(dev);
}
