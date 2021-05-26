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

#include <linux/gpio.h>
#include <linux/delay.h>

#define IOCTL_START_NUM 0x80 // 매직넘버 다른 값을 쓰는게 좋다
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

#define DEV_NAME "ku_act_dev"

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define STEPS 8
#define ONEROUND 512

int blue[8] = {1, 1, 0, 0, 0, 0, 0, 1};
int pink[8] = {0, 1, 1, 1, 0, 0, 0, 0};
int yellow[8] = {0, 0, 0, 1, 1, 1, 0, 0};
int orange[8] = {0, 0, 0, 0, 0, 1, 1, 1};

static void setstep(int p1, int p2, int p3, int p4){
    gpio_set_value(PIN1, p1);
    gpio_set_value(PIN2, p2);
    gpio_set_value(PIN3, p3);
    gpio_set_value(PIN4, p4);
}


MODULE_LICENSE("GPL");
spinlock_t my_lock;


void forward(int round, int delay){
    int i = 0, j = 0;
    for(i = 0;i<ONEROUND * round; i++){
        for(j=0;j<STEPS;j++){
            setstep(blue[j],pink[j], yellow[j], orange[j]);
            udelay(delay);
        }
    }
    setstep(0,0,0,0);
}

static void ku_act_dev_write(struct file *file, unsigned long arg){
	int ret;
	int write_data;

	spin_lock(&my_lock);
	copy_from_user(&write_data, (int *)arg, sizeof(int));
	printk("ku_act_dev: write data is %d\n", write_data);

	spin_unlock(&my_lock);
	
	if(write_data > 25){
		forward(1, 900);
	}

	return ret;
}

static long ku_act_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case IOCTL_WRITE:
			printk("ku_act_ioctl: IOCTL_WRITE. %ld\n", arg);
			ku_act_dev_write(file, arg);
			break;
		default:
			return -1;
	}

	return 0;
}

struct file_operations ku_act_dev_fops = {
	.unlocked_ioctl = ku_act_ioctl
};



static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_act_dev_init(void){
	int ret;

	printk("ku_act_dev: init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &ku_act_dev_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("ku_act_dev: fail to add character device \n");
		return -1;
	}

	// motor
    gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
    gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
    gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
    gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");
	
	
	spin_lock_init(&my_lock);


	return 0;
	
}

static void __exit ku_act_dev_exit(void)
{
	printk("ku_act_dev: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다

    gpio_free(PIN1);
    gpio_free(PIN2);
    gpio_free(PIN3);
    gpio_free(PIN4);
}

module_init(ku_act_dev_init);
module_exit(ku_act_dev_exit);
