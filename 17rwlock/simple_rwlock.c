#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/rwlock.h>


MODULE_LICENSE("GPL");

#define DEV_NAME "simple_rwlock_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static dev_t dev_num;
static struct cdev *cd_cdev;
rwlock_t my_lock;

unsigned long *my_data; // shared value

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    unsigned long flags;
    unsigned long *new, *old;

	switch(cmd){
        case IOCTL_READ:
            read_lock_irqsave(&my_lock, flags);
            old = my_data;
            printk("simple_rwlock : Read my_data = %ld\n", *old);
            mdelay(500); // 0.5sec sleep
			printk("simple_rwlock : After delay, Read my_data = %ld\n", *old);
			read_unlock_irqrestore(&my_lock,flags);
            break;
		case IOCTL_WRITE:
            write_lock_irqsave(&my_lock, flags);
            printk("simple_rwlock : write new data = %ld\n", arg);
            new = (unsigned long*)kmalloc(sizeof(unsigned long), GFP_KERNEL); // make new space
            *new = arg; // store new value
            old = my_data;
            my_data = new; // change pointer value
            kfree(old); // free old space
            write_unlock_irqrestore(&my_lock, flags);
			break;
        default:
            return -1;
	}

	return 0;
}

static int simple_rwlock_open(struct inode *inode, struct file *file){
    printk("simple_rwlock : open \n");
    return 0;
}

static int simple_rwlock_release(struct inode *inode, struct file *file){
    printk("simple_rwlock : close \n");
    return 0;
}

struct file_operations simple_rwlock_fops = {
    .unlocked_ioctl = simple_ioctl,
    .open = simple_rwlock_open,
    .release = simple_rwlock_release,
};

static int __init simple_rwlock_init(void){

    printk("simple_rwlock: init modules \n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_rwlock_fops);
    cdev_add(cd_cdev, dev_num, 1);

    rwlock_init(&my_lock);

    my_data = (unsigned long*)kmalloc(sizeof(unsigned long), GFP_KERNEL);
    *my_data = 0; // init shared data

    printk("simple_rwlock : init module\n");

    return 0;
}

static void __exit simple_rwlock_exit(void){
    printk("simple_rwlock: exit modules \n");

    kfree(my_data);
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);

}

module_init(simple_rwlock_init);
module_exit(simple_rwlock_exit);
