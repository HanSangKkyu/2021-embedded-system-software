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

#define DEV_NAME "ku_ipc_dev"

#define MAX_TIMING 85
#define DHT11 21

static int dht11_data[5] = {0,};
static bool isStop = false;

struct msg_list{
	struct list_head list;
	int msg;
};

static struct msg_list msg_list_head;


MODULE_LICENSE("GPL");
int *kern_buf;
spinlock_t my_lock;
wait_queue_head_t my_wq;
static long my_data;

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
			printk("set temp %d",tmp->msg);
			spin_unlock(&my_lock);
		}


		// // 해당 링크드 리스트에 유휴공이 있으면
		// spin_lock(&my_lock);
	
		// tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
		// // tmp->id = i;
		// tmp->msg = dht11_data[2];
		// // tmp->msg = *((struct msgbuf*)msgp);

		// printk("ku_ipc: enter to list %d\n", &(tmp->msg));
		// list_add(&tmp->list, &msg_list_head.list);

		// my_data = 1; // my_data에 데이터를 넣어서 my_wq에 잠들어 있는 프로세스를 깨운다

		// spin_unlock(&my_lock);


		// wake_up_interruptible(&my_wq);
		// my_data = 0;
	
	}else{
        printk("Data not good, skip\n");
    }
}

static ssize_t ku_ipc_read(struct file *file, char *buf, size_t len, loff_t *lof){
	int ret;
	char kern_buf = 'h';
	struct list_head *pos = NULL;
	struct msg_list *tmp = NULL;


	printk("simple_char: read\n");

	dht11_read();


	list_for_each(pos, &(msg_list_head.list)){
		tmp = list_entry(pos, struct msg_list, list);
		if(tmp->msg == NULL){
			printk("no data here\n");
		}else{
			printk("send temp is %d", tmp->msg);
			kern_buf = (tmp->msg)+'0';
			spin_lock(&my_lock);
			ret = copy_to_user(buf, &kern_buf, sizeof(char));
			// memset(kern_buf, '\0', sizeof(struct str_st));
			spin_unlock(&my_lock);
		}
	}

	// isStop = true;
	return ret;
}


struct file_operations ku_ipc_fops = {
	.read = ku_ipc_read
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ku_ipc_init(void){
	int ret;
	struct msg_list *tmp = NULL;

	printk("ku_ipc: init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &ku_ipc_fops);// cdev에 file operation을 할당한다.
	ret = cdev_add(cd_cdev, dev_num, 1);
	if(ret < 0){
		printk("fail to add character device \n");
		return -1;
	}

	INIT_LIST_HEAD(&msg_list_head.list);

	
	
	spin_lock_init(&my_lock);
	init_waitqueue_head(&my_wq);

	// kern_buf = (struct msgbuf*)vmalloc(sizeof(struct msgbuf));
	// memset(kern_buf, '\0', sizeof(struct msgbuf));
    gpio_request(DHT11, "DHT11");

	tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	tmp->msg = NULL;
	printk("ku_ipc: init list %d\n", &(tmp->msg));
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

static void __exit ku_ipc_exit(void)
{
	printk("ku_ipc: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다
	vfree(kern_buf);

	gpio_set_value(DHT11, 0);
    gpio_free(DHT11);
}

module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
