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
static int reference_counter[KUIPC_MAXMSG];


MODULE_LICENSE("GPL");
struct msgbuf *kern_buf;
spinlock_t my_lock;
wait_queue_head_t my_wq;

static int getSize(struct msg_list linked_list){
	struct list_head *pos = NULL;

	int ret = 0;
	list_for_each(pos, &linked_list.list){
		// tmp = list_entry(pos, struct msg_list, list);
		// printk("simple_linked_list: pos[%d], id[%d] \n",i,tmp->id);
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

static int ku_ipc_write(struct file *file, int msqid, void *msqp, int msgsz, int msgflg){

	int ret;
	struct msg_list *tmp = NULL;

	// unsigned int i = 0;

	if(getSize(msg_list_head[msqid]) < KUIPC_MAXVOL){
		// 해당 링크드 리스트에 유휴공간이 있으면
		spin_lock(&my_lock);
	
		copy_from_user(kern_buf, msqp, sizeof(struct msgbuf));

		tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
		// tmp->id = i;
		tmp->msg = *kern_buf;
		printk("ku_ipc: enter to list [%d] %s\n", msqid, (tmp->msg.text));
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
			ret = wait_event_interruptible(my_wq, getSize(msg_list_head[msqid]) < KUIPC_MAXVOL);
			// getSize(msg_list_head[msqid]) < KUIPC_MAXVOL 이 만족할 때까지 잠들겠다
			// msg_list_head[msqid]에 있는 데이터를 가져갈 때까지 잠든다
			if(ret < 0)
				return ret;
			
			return 0;
		}
	}

	return ret;
}


// static ssize_t simple_char_read(struct file *file, char *buf, size_t len, loff_t *lof){
// 	printk("simple_char: read\n");
// 	return 0;
// }
static int ku_ipc_read(struct file *file, int msqid, void *msqp, int msgsz, long msgtyp, int msgflg){

	struct msg_list *tmp = NULL;
	struct list_head *pos = NULL;
	struct list_head *q = NULL;

	char tmp_sring[KUIPC_MAXVOL];

	int ret;
	unsigned int i = 0;

	int msgSize = getSize(msg_list_head[msqid]);


	if(msgSize == 0){
		// 읽을 데이터가 없을 때
		if(msgflg == KU_IPC_NOWAIT){
			// 읽을 데이터가 들어오길 기다리지 않는다.
			return -1;
		}else{
			// 읽을 데이터가 들어오길 기다린다.
			ret = wait_event_interruptible(my_wq, getSize(msg_list_head[msqid]) > 0);
			
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
						ret = copy_to_user(msqp, tmp_sring, sizeof(struct msgbuf));
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
				ret = copy_to_user(msqp, tmp_sring, sizeof(struct msgbuf));
				memset(kern_buf, '\0', sizeof(struct msgbuf));
				spin_unlock(&my_lock);
				list_del(pos);
				kfree(tmp);
				i++;
			}
		}


	}
	return ret;
}

static long ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	// struct msgbuf *user_buf;
	// int ret;

	int key = (int)arg;

	switch(cmd){
		case KU_IPC_CREAT:
			// reference counter ++
			reference_counter[key] ++;
			printk("ku_ipc: KU_IPC_CREAT key:%d value:%d\n", key, reference_counter[key]);
			return key;
			break;
		case KU_IPC_EXCL:
			printk("ku_ipc: KU_IPC_EXCL key:%d value:%d\n", key, reference_counter[key]);
				
			if(reference_counter[key] == 0){
				// 사용 중이지 않다면
				reference_counter[key] ++;
				return key;
			}else{
				// 사용 중이라면 
				return -1;
			}
			
			break;
	}

	return 0;
}

static int ku_ipc_open(struct inode *inode, struct file *file){
	// printk("ku_ipc: open\n");
	return 0;
}

static int ku_ipc_release(struct inode *inode, struct file *file, int msqid){
	printk("ku_ipc: release\n");

	if(reference_counter[msqid] > 0){
		// 이미 사용 중이라면 
		reference_counter[msqid] --;
		return 0;
	}else{
		// 사용하고 있지 않다면
		return -1;
	}
}

struct file_operations ku_ipc_fops = {
	.unlocked_ioctl = ku_ipc_ioctl,
	.read = ku_ipc_read,
	.write = ku_ipc_write,
	.open = ku_ipc_open,
	.release = ku_ipc_release
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_ipc_init(void){
	int idx;
	int ret;
	printk("ku_ipc: init module\n");

	for(idx = 0;idx<KUIPC_MAXMSG;idx++){
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
