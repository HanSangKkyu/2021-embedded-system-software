#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/rculist.h>


MODULE_LICENSE("GPL");

#define DEV_NAME "simple_rcu_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static dev_t dev_num;
static struct cdev *cd_cdev;
spinlock_t my_lock;

unsigned long __rcu *my_data; //  shared rcu value

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    unsigned long flags;
    unsigned long *new, *old;

	switch(cmd){
        case IOCTL_READ:
            rcu_read_lock();
            old = rcu_dereference(my_data);
            printk("simple_rcu : Read my_data = %ld\n", *old);
            mdelay(500); // 0.5sec sleep
			printk("simple_rcu : After delay, Read my_data = %ld\n", *old);
            rcu_read_unlock();
            break;
		case IOCTL_WRITE:
            spin_lock_irqsave(&my_lock, flags);
            printk("simple_rcu : write new data = %ld\n", arg);
            new = (unsigned long*)kmalloc(sizeof(unsigned long), GFP_KERNEL); // make new space
            *new = arg; // store new value
            old = rcu_dereference(my_data);
            rcu_assign_pointer(my_data, new);
            synchronize_rcu();
            kfree(old); // free old space
            spin_unlock_irqrestore(&my_lock, flags);
			break;
        default:
            return -1;
	}

	return 0;
}

static int simple_rcu_open(struct inode *inode, struct file *file){
    printk("simple_rcu : open \n");
    return 0;
}

static int simple_rcu_release(struct inode *inode, struct file *file){
    printk("simple_rcu : close \n");
    return 0;
}

struct file_operations simple_rcu_fops = {
    .unlocked_ioctl = simple_ioctl,
    .open = simple_rcu_open,
    .release = simple_rcu_release,
};

static int __init simple_rcu_init(void){

    unsigned long *data;
    
    printk("simple_rcu: init modules \n");


    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_rcu_fops);
    cdev_add(cd_cdev, dev_num, 1);

    spin_lock_init(&my_lock);

    data = (unsigned long*)kmalloc(sizeof(unsigned long), GFP_KERNEL);
    *data = 0; // init shared data
    rcu_assign_pointer(my_data,data);

    printk("simple_rcu : init module\n");

    return 0;
}

static void __exit simple_rcu_exit(void){
    unsigned long flags;
    unsigned long *data;

    printk("simple_rcu: exit modules \n");

    spin_lock_irqsave(&my_lock,flags);
    data = rcu_dereference(my_data);
    rcu_assign_pointer(my_data, NULL);
    kfree(my_data);
    spin_unlock_irqrestore(&my_lock, flags);

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(simple_rcu_init);
module_exit(simple_rcu_exit);
