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

#include "ch3.h"
#define DEV_NAME "ch3_dev"

struct msg_list{
	struct list_head list; // 더블 포인터가 달린다.
	struct msg_st msg;
	int id;
};

static struct msg_list msg_list_head; // 링크드 리스트의 헤더를 나타내는 노드이다


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

	// 유저 버퍼에 있는 데이터를 커널 버퍼로 옮긴다
	ret = copy_from_user(kern_buf, buf, sizeof(struct msg_st));
	// 커널 버퍼에 데이터가 이미 있어도 덮어쓰기 하나보다

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
			tmp = list_entry(pos, struct msg_list, list); // 링크드 리스트의 하나의 노드를 가져온다
			printk("ch3: free pos[%d], id[%d] %d %s", i , tmp->id, tmp->msg.len, (tmp->msg.str));
			*kern_buf = tmp->msg; // 가져온 노드를 커널 버퍼에 넣는다.
			delay(5);
			spin_lock(&my_lock);
			ret = copy_to_user(buf, kern_buf, sizeof(struct msg_st)); // 커널 버퍼의 데이터를 유저 버퍼로 옮긴다
			memset(kern_buf, '\0', sizeof(struct msg_st)); // 커널 버퍼를 비워준다
			spin_unlock(&my_lock);
			list_del(pos);
			kfree(tmp);
		}
		i++;
	}



	return ret;
}

static long simple_ch3_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct msg_st *user_buf;
	int ret;

	user_buf = (struct msg_st*)arg;

	switch(cmd){
		case CH3_IOCTL_WRITE:
			ret = simple_ch3_write(user_buf);
			printk("ch3: ioctl write return %d\n", ret);
			break;
		case CH3_IOCTL_READ:
			ret = simple_ch3_read(user_buf);
			printk("ch3: ioctl read return %d\n", ret);
			break;
	}

	return 0;
}

static int simple_spin_open(struct inode *inode, struct file *file){
	printk("ch3: open\n");
	return 0;
}

static int simple_spin_release(struct inode *inode, struct file *file){
	printk("ch3: release\n");
	return 0;
}

struct file_operations simple_spin_fops = {
	.unlocked_ioctl = simple_ch3_ioctl,
	.open = simple_spin_open,
	.release = simple_spin_release
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_spin_init(void){
	printk("ch3: init module\n");
	INIT_LIST_HEAD(&msg_list_head.list); // 자동으로 링크드 리스트가 되게 만들어 준다


	int ret;

	printk("ch3: init module\n");
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &simple_spin_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("fail to add character device \n");
		return -1;
	}

	kern_buf = (struct msg_st*)vmalloc(sizeof(struct msg_st)); // 커널 버퍼에 공간을 할당한다
	memset(kern_buf, '\0', sizeof(struct msg_st));

	return 0;
	
}

static void __exit simple_spin_exit(void)
{
	printk("ch3: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다
	vfree(kern_buf);
}

module_init(simple_spin_init);
module_exit(simple_spin_exit);
