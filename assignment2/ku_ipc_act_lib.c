#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

// #define DEV_NAME "ku_ipc_act_dev"


static int ku_write(int temp){
	int dev;
	int ret;
	char str[2] = "00";
	dev = open("/dev/ku_ipc_act_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	printf("dev%d\n", dev);
	str[0] = temp+'0';
	ret = write(dev, str, 10);
	return ret;
}
