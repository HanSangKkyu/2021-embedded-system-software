#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEV_NAME "ku_ipc_dev"

static int ku_read(char *stst){
	int dev;
	int ret;
	char rcv_data;
	int temp;
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = read(dev, stst, sizeof(int));
	printf("%c\n", *stst);
	// rcv_data = *stst;
	// printf("%c\n", rcv_data);
	// temp = atoi(rcv_data);

	// temp = (int)rcv_data;
	// printf("%d\n", &temp);

	return ret;
}
