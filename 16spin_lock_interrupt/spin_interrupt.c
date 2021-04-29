#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>


MODULE_LICENSE("GPL");

#define SENSOR1 17
#define DEV_NAME "spin_interrupt_dev"

static int irq_num;

struct time_list{
    struct list_head list;
    struct timespec time;
};

struct time_list time_list_head;
spinlock_t spinlock;

static int spin_interrupt_open(struct inode *inode, struct file *file){
    printk("spin_interrupt : open \n");
    enable_irq(irq_num);
    return 0;
}

static int spin_interrupt_release(struct inode *inode, struct file *file){
    struct time_list* node = 0;
    struct list_head* pos  = 0;
    struct list_head* q = 0;
    int index = 0;
    unsigned long flags;

    printk("spin_interrupt : show all\n");

    if(!list_empty(&time_list_head.list)){
        spin_lock_irqsave(&spinlock, flags);
        list_for_each_safe(pos, q, &time_list_head.list){
            node = list_entry(pos, struct time_list, list);
            printk("[%d] Time : %ld\n", index++, node->time.tv_sec);
        }
        spin_unlock_irqrestore(&spinlock, flags);
    }else{
        printk("Empty List\n");
    }

    printk("spin_interrupt : close\n");
    disable_irq(irq_num);
    return 0;
}

struct file_operations spin_interrupt_fops = {
    .open = spin_interrupt_open,
    .release = spin_interrupt_release,
};

static irqreturn_t spin_interrupt_isr(int irq, void *dev_id){
    unsigned long flags;
    struct time_list* node = 0;

    node = (struct time_list*)kmalloc(sizeof(struct time_list), GFP_ATOMIC);

    spin_lock_irqsave(&spinlock, flags);
    getnstimeofday(&node->time);
    printk("spin_interrupt : Detect \n");
    list_add_tail(&node->list, &time_list_head.list);
    spin_unlock_irqrestore(&spinlock, flags);
    return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init spin_interrupt_init(void){
    int ret;
    printk("spin_interrupt: init modules \n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &spin_interrupt_fops);
    cdev_add(cd_cdev, dev_num, 1);

    gpio_request_one(SENSOR1, GPIOF_IN, "sensor1");
    irq_num = gpio_to_irq(SENSOR1);
    ret = request_irq(irq_num, spin_interrupt_isr, IRQF_TRIGGER_RISING, "sensor_irq", NULL);
    if(ret){
        printk("spin_interrupt : Unable to reset request IRQ : %d\n", irq_num);
        free_irq(irq_num, NULL);
    }else{
        disable_irq(irq_num);
    }

    return 0;
}

static void __exit spin_interrupt_exit(void){
    struct time_list* node = 0;
    struct list_head* pos  = 0;
    struct list_head* q = 0;

    printk("spin_interrupt: exit modules \n");
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);

    free_irq(irq_num, NULL);
    gpio_free(SENSOR1);

    list_for_each_safe(pos, q, &time_list_head.list){
        node = list_entry(pos, struct time_list, list);

        list_del(pos);
        kfree(node);
    }
}

module_init(spin_interrupt_init);
module_exit(spin_interrupt_exit);
