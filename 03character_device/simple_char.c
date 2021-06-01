#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <asm/delay.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "simple_char_dev"

static int simple_char_open(struct inode *inode, struct file *file) {
	printk("simple_char: open\n");
	return 0;
}

static int simple_char_release(struct inode *inode, struct file *file) {
        printk("simple_char: release\n");
        return 0;
}

static ssize_t simple_char_read(struct file *file, char *buf, size_t len, loff_t *lof){
	int ret;
	char kern_buf = 'h';


	printk("simple_char: read\n");


	// spin_lock(&my_lock);
	ret = copy_to_user(buf, &kern_buf, sizeof(char));
	// memset(kern_buf, '\0', sizeof(struct str_st));
	// spin_unlock(&my_lock);


	return ret;
}

static ssize_t simple_char_write(struct file *file, const char *buf, size_t len, loff_t *lof){
        printk("simple_char: write%s\n", buf);
        return len;
}

struct file_operations simple_char_fops = {
	.open = simple_char_open,
	.release = simple_char_release,
	.read = simple_char_read,
	.write = simple_char_write,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_char_init(void){
	printk("simple_char: init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &simple_char_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return 0;
}

static void __exit simple_char_exit(void){
	printk("simple_char: exit module\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}

module_init(simple_char_init);
module_exit(simple_char_exit);
