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
#define DEV_NAME "ku_ipc_dev"

struct msgbuf {
	long type;
	char text[1];
	// char text[128];
};

struct msg_list{
	struct list_head list;
	struct msgbuf msg;
};

static struct msg_list msg_list_head[KUIPC_MAXMSG];


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

	switch(msgflg){
		case KU_IPC_CREAT:
			// key값의 큐 ID를 반환
			// ret = write(dev, msqid, msqp, msgsz, msgflg); // 시스템콜 호출
			dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
			ret = ioctl(dev, KU_IPC_CREAT, key);

			break;
		case KU_IPC_EXCL:
			// key값의 큐가 이미 사용중이라면 -1을 반환 어떤 프로세스가 사용중이지 않다면 return ID
			dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
			ret = ioctl(dev, KU_IPC_EXCL, key);
			break;
	}
	
	return ret;
}

static int ku_msgclose(int msqid){
	return 0;
	return -1;
}

static int ku_msgsnd(int msqid, void *msqp, int msgsz, int msgflg){
	int ret;
	int dev;
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = write(dev, msqid, msqp, msgsz, msgflg); // 시스템콜 호출
	close(dev);
	return ret;
}

static int ku_msgrcv(int msqid, void *msqp, int msgsz, long msgtyp, int msgflg){
	int ret;
	int dev;
	dev = open("/dev/ku_ipc_dev", O_RDWR); // 이 디바이스 드라이버를 사용하겠다
	ret = read(dev, msqid, msqp, msgsz, msgflg); // 시스템콜 호출
	print("읽은 값은 : %s", &msqp);
	close(dev);
	return ret;
}
