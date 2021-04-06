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
	void *msgp;
	int msgsz;
	int msgflg;
};

struct msgrcv_args {
	int msqid;
	void *msgp;
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
static int reference_counter[10];


MODULE_LICENSE("GPL");
struct msgbuf *kern_buf;
spinlock_t my_lock;
wait_queue_head_t my_wq;

static int getSize(struct list_head* list){
	struct list_head *pos = NULL;

	int ret = 0;

	// printk("%ld",list);
	list_for_each(pos, list){
		ret ++;
	}

	return ret;
}

void delay(int sec){
	int i,j;
	for(j = 0; j< sec;j++){
		for(i = 0;i<1000;i++){
			udelay(1000);
		}
	}
}

static int ku_msgget(int key, int msgflg){
	unsigned long my_msgflg = (unsigned long)msgflg;
	printk("ku_ipc: KU_IPC_CREAT my_msgflg:%ld \n", my_msgflg);

	switch(my_msgflg){
		case KU_IPC_CREAT:
			reference_counter[key] ++;
			printk("ku_ipc: KU_IPC_CREAT key:%d value:%d\n", key, reference_counter[key]);
			return key; // copy to user
			break;
		case KU_IPC_EXCL:
			printk("ku_ipc: KU_IPC_EXCL key:%d value:%d\n", key, reference_counter[key]);
			if(reference_counter[key] == 0){
				// 사용 중이지 않다면
				reference_counter[key] ++;
				return key;
			}else{
				// 사용 중이라면 
				printk("ku_ipc: return -1", key, reference_counter[key]);
				return -1;
			}
			break;
	}
	return 0;
}

static int ku_msgclose(int msqid){
	if(reference_counter[msqid] > 0){
		// 이미 사용 중이라면 
		reference_counter[msqid] --;
		printk("ku_ipc: ku_msgclose key:%d value:%d\n", msqid, reference_counter[msqid]);
		return 0;
	}else{
		// 사용하고 있지 않다면
		printk("ku_ipc: ku_msgclose not used");
		return -1;
	}
} 

static int ku_msgsnd(int msqid, void *msgp, int msgsz, int msgflg){

	int ret;
	struct msg_list *tmp = NULL;

	printk("ku_msgsnd: %d",getSize(&(msg_list_head[msqid].list)));

	if(getSize(&(msg_list_head[msqid].list)) < KUIPC_MAXMSG){
		// 해당 링크드 리스트에 유휴공이 있으면
		spin_lock(&my_lock);
	
		copy_from_user(kern_buf, (struct msgbuf*)msgp, sizeof(struct msgbuf));

		tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
		// tmp->id = i;
		tmp->msg = *kern_buf;
		// tmp->msg = *((struct msgbuf*)msgp);

		printk("ku_ipc: enter to list [%d] %c\n", msqid, (tmp->msg.text[0]));
		list_add_tail(&tmp->list, &msg_list_head[msqid].list);

		spin_unlock(&my_lock);
		return 0;
	}else{
		// 메시지 큐에 유휴공간 없으면
		if(msgflg == KU_IPC_NOWAIT){
			return -1;
		}
		else if(msgflg == 0){
			//block 시키자 해당 프로세스 재우기
			// 어떤 프로세스가 메시지 큐를 비우면 그 때 메세지를 보내자
			ret = wait_event_interruptible(my_wq, getSize(&(msg_list_head[msqid].list)) < KUIPC_MAXMSG);
			// getSize(msg_list_head[msqid]) < KUIPC_MAXVOL 이 만족할 때까지 잠들겠다
			// msg_list_head[msqid]에 있는 데이터를 가져갈 때까지 잠든다
			if(ret < 0)
				return ret;
			
			return 0;
		}
	}

	return ret;
}

static int ku_msgrcv(int msqid, void *msgp, int msgsz, long msgtyp, int msgflg){

	struct msg_list *tmp = NULL;
	struct list_head *pos = NULL;
	struct list_head *q = NULL;

	char tmp_sring[KUIPC_MAXMSG];

	int ret;
	unsigned int i = 0;

	int msgSize = getSize(&(msg_list_head[msqid].list));


	if(msgSize == 0){
		// 읽을 데이터가 없을 때
		if(msgflg == KU_IPC_NOWAIT){
			// 읽을 데이터가 들어오길 기다리지 않는다.
			return -1;
		}else{
			// 읽을 데이터가 들어오길 기다린다.
			ret = wait_event_interruptible(my_wq, getSize(&(msg_list_head[msqid].list)) > 0);
			
		}
	}else{
		// 읽을 데이터가 있을 때
		if(msgSize > msgsz){
			// 읽을 데이터가 msgsz 보다 클 때
			if(msgflg == KU_MSG_NOERROR){
				// 읽을 수 있는 만큼만 보내기
				list_for_each_safe(pos, q, &msg_list_head[msqid].list){
					if(i < msgsz){
						tmp = list_entry(pos, struct msg_list, list);
						// printk("ku_ipc: free pos[%d], id[%d] %d %s", i , tmp->id, tmp->msg.len, (tmp->msg.str));
						*kern_buf = tmp->msg;
						tmp_sring[i] = kern_buf->text[0];
						delay(5);
						spin_lock(&my_lock);
						ret = copy_to_user(msgp, tmp_sring, sizeof(struct msgbuf));
						memset(kern_buf, '\0', sizeof(struct msgbuf));
						spin_unlock(&my_lock);
						list_del(pos);
						kfree(tmp);
						i++;
					}
				}

			}else{
				// 바로 리턴
				return -1;

			}
		}else{
			// 읽을 데이터가 msgsz보다 작을 때
			list_for_each_safe(pos, q, &msg_list_head[msqid].list){
				tmp = list_entry(pos, struct msg_list, list);
				// printk("ku_ipc: free pos[%d], id[%d] %d %s", i , tmp->id, tmp->msg.len, (tmp->msg.str));
				*kern_buf = tmp->msg;
				tmp_sring[i] = kern_buf->text[0];
				delay(5);
				spin_lock(&my_lock);
				// ret = copy_to_user(msgp, tmp_sring, sizeof(struct msgbuf));
				memset(kern_buf, '\0', sizeof(struct msgbuf));
				spin_unlock(&my_lock);
				list_del(pos);
				kfree(tmp);
				i++;
			}

			ret = copy_to_user(msgp, tmp_sring, sizeof(struct msgbuf));
		}


	}
	return ret;
}

static long ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	// printk("ku_ipc_ioctl: %d\n",KU_IPC_CREAT);
	// printk("ku_ipc_ioctl: %ld\n",cmd);


	struct msgget_args *my_msgget_args;
	struct msgclose_args *my_msgclose_args;
	struct msgsnd_args *my_msgsnd_args;
	struct msgrcv_args *my_msgrcv_args;
	int ret;

	switch(cmd){
		// ku_msgget
		case KU_MSGGET:
			my_msgget_args = (struct msgget_args*)arg;
			// printk("ku_ipc_ioctl: %d\n",my_msgget_args->key);
			ret = ku_msgget(my_msgget_args->key, my_msgget_args->msgflg);
			// printk("ku_ipc_ioctl: %d\n",ret);
			// return ret;
			break;
		// ku_msgclose
		case KU_MSGCLOSE:
			my_msgclose_args = (struct msgclose_args*)arg;
			// printk("ku_ipc_ioctl: %d\n",my_msgclose_args->msqid);
			ret = ku_msgclose(my_msgclose_args->msqid);
			break;

		// ku_msgsnd
		case KU_MSGSND:
			my_msgsnd_args = (struct msgsnd_args*)arg;
			// printk("ku_ipc_ccc: %d %c %d %d",my_msgsnd_args->msqid, ((struct msgbuf*)(my_msgsnd_args->msgp))->text[0], my_msgsnd_args->msgsz, my_msgsnd_args->msgflg);
			ret = ku_msgsnd(my_msgsnd_args->msqid, my_msgsnd_args->msgp, my_msgsnd_args->msgsz, my_msgsnd_args->msgflg);
			break;

		// ku_msgrcv
		case KU_MSGRCV:
			my_msgrcv_args = (struct msgrcv_args*)arg;
			ret = ku_msgrcv(my_msgrcv_args->msqid, my_msgrcv_args->msgp, my_msgrcv_args->msgsz, my_msgrcv_args->msgtyp, my_msgrcv_args->msgflg);
			break;
	}

	return ret;
	// return 0;
}

static int ku_ipc_open(struct inode *inode, struct file *file){
	printk("ku_ipc: open\n");
	return 0;
}

static int ku_ipc_release(struct inode *inode, struct file *file){
	printk("ku_ipc: release\n");
	return 0;
}

struct file_operations ku_ipc_fops = {
	.unlocked_ioctl = ku_ipc_ioctl,
	.open = ku_ipc_open,
	.release = ku_ipc_release
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_ipc_init(void){
	int idx;
	int ret;
	printk("ku_ipc: init module\n");

	for(idx = 0;idx<10;idx++){
		INIT_LIST_HEAD(&msg_list_head[idx].list); // 큐 배열 초기화
		reference_counter[idx] = 0; // reference counter 초기화
	}

	printk("ku_ipc: init module\n");
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &ku_ipc_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("fail to add character device \n");
		return -1;
	}

	kern_buf = (struct msgbuf*)vmalloc(sizeof(struct msgbuf));
	memset(kern_buf, '\0', sizeof(struct msgbuf));

	return 0;
	
}

static void __exit ku_ipc_exit(void)
{
	printk("ku_ipc: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다
	vfree(kern_buf);
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
