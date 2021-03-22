#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
// #include "simple_ioctl.h"

#define IOCTL_START_NUM 0x80 // 매직넘버 다른 값을 쓰는게 좋다
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
// #define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
// #define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define SIMPLE_IOCTL1 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SIMPLE_IOCTL2 _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

void main(void){
	int dev;
	unsigned long value = 200;
	
	dev = open("/dev/simple_ioctl_dev", O_RDWR);
	
	ioctl(dev, SIMPLE_IOCTL1, value);
	ioctl(dev, SIMPLE_IOCTL2, &value);
	
	close(dev);
}
