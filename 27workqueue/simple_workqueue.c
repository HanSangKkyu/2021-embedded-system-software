#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

#define SWITCH 17

struct my_data{
    int year;
    int month;
    int day;
};

static struct workqueue_struct *my_wq;

typedef struct{
    struct work_struct my_work;
    struct my_data data;
}my_work_t;

my_work_t * work;

static int irq_num;

void my_work_func(struct work_struct *work){
    my_work_t *my_work = (my_work_t*) work;

    printk("simple_workqueue: my work start \n");

    msleep(1000);

    (my_work->data).year = 2021;
    (my_work->data).month = 5;
    (my_work->data).day = 20;

    printk("simple_workqueue: today is %d/%d/%d \n", (my_work->data).year, (my_work->data).month, (my_work->data).day);

    kfree((void*)work);
}

static irqreturn_t simple_workqueue_isr(int irq, void* data){
    int ret;
    printk("simple_workqueue: interrupt isr\n");

    if(my_wq){
        work = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
        if(work){
            INIT_WORK((struct work_struct*)work, my_work_func);
            ret = queue_work(my_wq, (struct work_struct*)work);
        }
    }

    return IRQ_HANDLED;
}

static int __init simple_workqueue_init(void){
    int ret;

    printk("simple_workqueue: init module \n");

    my_wq = create_workqueue("my_workqueue");

    gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");
    irq_num = gpio_to_irq(SWITCH);
    ret = request_irq(irq_num, simple_workqueue_isr, IRQF_TRIGGER_FALLING, "switch_irq", NULL);

    if(ret){
        printk("Unable to request IRQ: %d\n",ret);
        free_irq(irq_num, NULL);
    }

    return 0;
}

static void __exit simple_workqueue_exit(void){
    free_irq(irq_num, NULL);
    gpio_free(SWITCH);

    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);

    printk("simple tasklet: exit module\n");
}

module_init(simple_workqueue_init);
module_exit(simple_workqueue_exit);
