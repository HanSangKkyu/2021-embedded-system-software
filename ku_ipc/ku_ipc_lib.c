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
	int id;
	struct list_head list;
	struct msgbuf msg[KUIPC_MAXVOL];
};

static struct msg_list msg_list_head[KUIPC_MAXMSG];


MODULE_LICENSE("GPL");
struct msg_st *kern_buf;
spinlock_t my_lock;

void delay(int sec){
	int i,j;
	for(j = 0; j< sec;j++){
		for(i = 0;i<1000;i++){
			udelay(1000);
		}
	}
}

static int simple_ch3_write(struct msg_st *buf){

	struct msg_list *tmp = NULL;

	int ret;
	unsigned int i = 0;

	spin_lock(&my_lock);
	
	ret = copy_from_user(kern_buf, buf, sizeof(struct msg_st));

	tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	tmp->id = i;
	tmp->msg = *kern_buf;
	printk("ch3: enter to list [%d] %d %s\n", tmp->id, tmp->msg.len , (tmp->msg.str));
	list_add_tail(&tmp->list, &msg_list_head.list);

	spin_unlock(&my_lock);

	return 0;
}

static int simple_ch3_read(struct msg_st *buf){

	struct msg_list *tmp = NULL;
	struct list_head *pos = NULL;
	struct list_head *q = NULL;

	int ret;
	unsigned int i = 0;

	list_for_each_safe(pos, q, &msg_list_head.list){
		if(i==0){
			tmp = list_entry(pos, struct msg_list, list);
			printk("ch3: free pos[%d], id[%d] %d %s", i , tmp->id, tmp->msg.len, (tmp->msg.str));
			*kern_buf = tmp->msg;
			delay(5);
			spin_lock(&my_lock);
			ret = copy_to_user(buf, kern_buf, sizeof(struct msg_st));
			memset(kern_buf, '\0', sizeof(struct msg_st));
			spin_unlock(&my_lock);
			list_del(pos);
			kfree(tmp);
		}
		i++;
	}



	return ret;
}