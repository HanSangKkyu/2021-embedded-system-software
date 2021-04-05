#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <asm/delay.h>

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>

#include "ku_ipc.h"

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2
#define IOCTL_NUM3 IOCTL_START_NUM + 3
#define IOCTL_NUM4 IOCTL_START_NUM + 4
#define IOCTL_NUM5 IOCTL_START_NUM + 5
#define IOCTL_NUM6 IOCTL_START_NUM + 6
#define IOCTL_NUM7 IOCTL_START_NUM + 7
#define IOCTL_NUM8 IOCTL_START_NUM + 8
#define KU_MSGGET _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM5, unsigned long)
#define KU_MSGCLOSE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM6, unsigned long)
#define KU_MSGSND _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM7, unsigned long)
#define KU_MSGRCV _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM8, unsigned long)


#define DEV_NAME "ku_ipc_dev"

struct msgget_args {
	int key;
	int msgflg;
};

struct msgclose_args {
	int msqid;
};

struct msgsnd_args {
	int msqid;
	void *msqp;
	int msgsz;
	int msgflg;
};

struct msgrcv_args {
	int msqid;
	void *msqp;
	int msgsz;
	long msgtyp;
	int msgflg;
};

struct msgbuf {
	long type;
	char text[1];
	// char text[128];
};

struct msg_list{
	struct list_head list;
	struct msgbuf msg;
};

static struct msg_list msg_list_head[10];


MODULE_LICENSE("GPL");
struct msgbuf *kern_buf;
spinlock_t my_lock;

void delay(int sec){
	int i,j;
	for(j = 0; j< sec;j++){
		for(i = 0;i<1000;i++){
			udelay(1000);
		}
	}
}

static int ku_msgget(int key, int msgflg){
	int dev;
	int ret;
	struct msgget_args my_args = {key, msgflg};
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = ioctl(dev, KU_MSGGET, my_args);
	return ret;
}

static int ku_msgclose(int msqid){
	int dev;
	int ret;
	struct msgclose_args my_args = {msqid};
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = ioctl(dev, KU_MSGCLOSE, my_args);
	return ret;
}

static int ku_msgsnd(int msqid, void *msqp, int msgsz, int msgflg){
	int dev;
	int ret;
	struct msgsnd_args my_args = {msqid,msqp,msgsz,msgflg};
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = ioctl(dev, KU_MSGSND, my_args);
	return ret;
}

static int ku_msgrcv(int msqid, void *msqp, int msgsz, long msgtyp, int msgflg){
	int dev;
	int ret;
	struct msgrcv_args my_args = {msqid,msqp,msgsz,msgtyp,msgflg};
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = ioctl(dev, KU_MSGRCV, my_args);
	return ret;
}
