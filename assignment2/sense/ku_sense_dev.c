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

#define DEV_NAME "ku_sense_dev"

#define MAX_TIMING 85
#define DHT11 21

static int dht11_data[5] = {0,};

struct msg_list{
	struct list_head list;
	int msg;
};

static struct msg_list msg_list_head;


MODULE_LICENSE("GPL");
spinlock_t my_lock;

static void dht11_read(void){
    int last_state = 1;
    int counter = 0;
    int i =0, j=0;
	struct list_head *pos = NULL;
	struct msg_list *tmp = NULL;


    dht11_data[0] = dht11_data[1] = dht11_data[2] = dht11_data[3] = dht11_data[4] = 0;

    gpio_direction_output(DHT11, 0);
    gpio_set_value(DHT11, 0);
    mdelay(18);
    gpio_set_value(DHT11, 1);
    udelay(40);
    gpio_direction_input(DHT11);

    for(i=0;j<MAX_TIMING; i++){
        counter = 0;
        while(gpio_get_value(DHT11) == last_state){
            counter ++;
            udelay(1);
            if(counter == 255){
                break;
            }
        }
        last_state = gpio_get_value(DHT11);

        if(counter == 255){
            break;
        }

        if(i>=4 && i%2 == 0){
            dht11_data[j/8] <<= 1;
            if(counter>16){
                dht11_data[j/8] |= 1;
            }
            j++;
        }
    }

    if((j>=40) && (dht11_data[4] == ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3])&0xFF))){
        printk("Humidity: %d.%d Temperature = %d.%d C\n", dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);

		list_for_each(pos, &(msg_list_head.list)){
			tmp = list_entry(pos, struct msg_list, list);
			spin_lock(&my_lock);
			tmp->msg = dht11_data[2];
			printk("ku_sense_dev: set temp %d",tmp->msg);
			spin_unlock(&my_lock);
		}

	
	}else{
        printk("ku_sense_dev: Data not good, skip\n");
    }
}

static void ku_sense_dev_read(struct file *file, unsigned long arg){
	// char kern_buf = 'h';
	unsigned long kern_buf;
	struct list_head *pos = NULL;
	struct msg_list *tmp = NULL;


	printk("ku_sense_dev: read\n");

	dht11_read();

	list_for_each(pos, &(msg_list_head.list)){
		tmp = list_entry(pos, struct msg_list, list);
		if(tmp->msg == NULL){
			printk("ku_sense_dev: no data here\n");
		}else{
			printk("ku_sense_dev: send temp is %d", tmp->msg);
			kern_buf = tmp->msg;
			spin_lock(&my_lock);
			copy_to_user((int *)arg, &kern_buf, sizeof(unsigned long));

			spin_unlock(&my_lock);
		}
	}

	return ;
}

static long ku_sense_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case IOCTL_READ:
			printk("ku_sense_ioctl: IOCTL_READ. %ld\n", arg);
			ku_sense_dev_read(file, arg);
			break;
		default:
			return -1;
	}

	return 0;
}

struct file_operations ku_sense_dev_fops = {
	.unlocked_ioctl = ku_sense_ioctl
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_sense_dev_init(void){
	int ret;
	struct msg_list *tmp = NULL;

	printk("ku_sense_dev: init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &ku_sense_dev_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("ku_sense_dev: fail to add character device \n");
		return -1;
	}

	INIT_LIST_HEAD(&msg_list_head.list);

	
	
	spin_lock_init(&my_lock);

    gpio_request(DHT11, "DHT11");

	tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	tmp->msg = NULL;
	printk("ku_sense_dev: init list %d\n", &(tmp->msg));
	list_add(&tmp->list, &msg_list_head.list);

	// call 
	// while(true){
	// 	if(isStop){
	// 		break;
	// 	}
	// 	dht11_read();
	// 	mdelay(1000);
	// }

	dht11_read();

	return 0;
	
}

static void __exit ku_sense_dev_exit(void)
{
	printk("ku_sense_dev: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다
	// vfree(kern_buf);

	gpio_set_value(DHT11, 0);
    gpio_free(DHT11);
}

module_init(ku_sense_dev_init);
module_exit(ku_sense_dev_exit);
