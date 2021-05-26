#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_NAME "ku_ipc_dev"

static int ku_read(char *stst){
	int dev;
	int ret;
	// char *stst;
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = read(dev, &stst, sizeof(int));
	printf("%c\n", stst);
	return ret;
}
