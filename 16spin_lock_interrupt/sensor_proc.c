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
#define DEV_NAME "simple_sensor_dev"

static int irq_num;
struct timespec start;
struct timespec recent;

static int simple_sensor_open(struct inode *inode, struct file *file){
    printk("simple_sensor : open \n");
    enable_irq(irq_num);
    getnstimeofday(&start);
    return 0;
}

static int simple_sensor_release(struct inode *inode, struct file *file){
    printk("simple_sensor : close \n");
    disable_irq(irq_num);
    return 0;
}

struct file_operations simple_sensor_fops = {
    .open = simple_sensor_open,
    .release = simple_sensor_release,
};

static irqreturn_t simple_sensor_isr(int irq, void *dev_id){
    unsigned long flags;
    local_irq_save(flags);
    getnstimeofday(&recent);
    printk("simple_sensor : Detect at %ld secs \n", recent.tv_sec - start.tv_sec);
    local_irq_restore(flags);
    return IRQ_HANDLED;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_sensor_init(void){
    int ret;

    printk("simple_sensor: init modules \n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_sensor_fops);
    cdev_add(cd_cdev, dev_num, 1);

    gpio_request_one(SENSOR1, GPIOF_IN, "sensor1");
    irq_num = gpio_to_irq(SENSOR1);
    ret = request_irq(irq_num, simple_sensor_isr, IRQF_TRIGGER_RISING, "sensor_irq", NULL);
    if(ret){
        printk("simple_sensor : Unable to reset request IRQ : %d\n", irq_num);
        free_irq(irq_num, NULL);
    }else{
        disable_irq(irq_num);
    }

    return 0;
}

static void __exit simple_sensor_exit(void){
    printk("simple_sensor: exit modules \n");
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);

    free_irq(irq_num, NULL);
    gpio_free(SENSOR1);
}

module_init(simple_sensor_init);
module_exit(simple_sensor_exit);
