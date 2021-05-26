#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_START_NUM 0x80 // 매직넘버 다른 값을 쓰는게 좋다
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static int ku_write(int temp){
	int dev;
	int ret;
	dev = open("/dev/ku_act_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	printf("dev%d\n", dev);
	ret = ioctl(dev, IOCTL_WRITE, &temp);

	return ret;
}
