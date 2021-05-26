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

#define DEV_NAME "ku_ipc_act_dev"

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

static bool isStop = false;


MODULE_LICENSE("GPL");
static char *kern_buf;
spinlock_t my_lock;
wait_queue_head_t my_wq;
static long my_data;


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

static ssize_t ku_ipc_write(struct file *file, const char *buf, size_t len, loff_t *lof){
	int ret;
	int write_data;

	spin_lock(&my_lock);
	// ret = copy_from_user(kern_buf, buf, 30);
	// kern_buf = "hh";
	// printk("write data is %s\n", kern_buf);
	write_data = (int)(buf[0])-(int)('0');
	printk("write data is %d\n", write_data);

	spin_unlock(&my_lock);
	
	if(write_data > 25){
		forward(1, 900);
	}

	return ret;
}

struct file_operations ku_ipc_fops = {
	.write = ku_ipc_write
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_ipc_init(void){
	int ret;

	printk("ku_ipc_act: init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &ku_ipc_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("fail to add character device \n");
		return -1;
	}

	// motor
    gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
    gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
    gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
    gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");
	
	
	spin_lock_init(&my_lock);
	init_waitqueue_head(&my_wq);


	return 0;
	
}

static void __exit ku_ipc_exit(void)
{
	printk("ku_ipc_act: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다

    gpio_free(PIN1);
    gpio_free(PIN2);
    gpio_free(PIN3);
    gpio_free(PIN4);
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
