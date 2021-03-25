#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <asm/delay.h>

#include "ch3.h"
#define DEV_NAME "ch3_dev"

struct msg_list{
	struct list_head list;
	struct msg_st msg;
	int id;
};

static struct msg_list msg_list_head;


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
	int ret;

	spin_lock(&my_lock); 
	
	ret = copy_from_user(kern_buf, buf, sizeof(struct msg_st));

	tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	tmp->id = i;
	tmp->msg = kern_buf;
	printk("simple_linked_list: enter to list [%d]\n", tmp->id);
	list_add_tail(&tmp->list, &msg_list_head.list);

	spin_unlock(&my_lock);

	return 0;
}

static int simple_ch3_read(struct msg_st *buf){
	int ret;
	unsigned int i = 0;

	list_for_each_safe(pos, q, &msg_list_head.list){
		tmp = list_entry(pos, struct msg_list, list);
		printk("simple_linked_list: free pos[%d], id[%d]", i , tmp->id);
		kern_buf = tmp->id;
		delay(5);
		spin_lock(&my_lock);
		ret = copy_to_user(buf, kern_buf, sizeof(struct msg_st));
		memset(kern_buf, '\0', sizeof(struct msg_st));
		spin_unlock(&my_lock);
		kfree(tmp);
		i++;
		break;
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
	struct test_list *tmp = 0;
	struct list_head *pos = 0;
	unsigned int i;
	printk("ch3: init module\n");
	INIT_LIST_HEAD(&msg_list_head.list);


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

	kern_buf = (struct msg_st*)vmalloc(sizeof(struct msg_st));
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
