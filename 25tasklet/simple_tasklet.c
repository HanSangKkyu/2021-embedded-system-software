#include <linux/init.h>
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

struct my_data my_tasklet_data;
struct tasklet_struct my_tasklet;
struct int irq_num;

void simple_tasklet_func(unsigned long recv_data){
    struct my_data *temp_data;
    temp_data = (struct my_data *) recv_data;

    printk("today is %d/%d/%d \n", temp_data->year, temp_data->month, temp_data->day);
}

static irqreturn_t simple_tasklet_isr(int irq, void* data){
    printk("simple_tasklet: interrupt isr\n");
    tasklet_schedule(&my_tasklet);

    return IRQ_HANDLED;
}

static int __init simple_tasklet_init(void){
    int ret = 0;

    printk("simple_tasklet: init module \n");

    my_tasklet_data.year = 2020;
    my_tasklet_data.month = 6;
    my_tasklet_data.day = 9;

    tasklet_init(&my_tasklet, simple_tasklet_func, (unsigned long) &my_tasklet_data);

    gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");
    irq_num = gpio_to_irq(SWITCH);
    ret = request_irq(irq_num, simple_tasklet_isr, IRQF_TRIGGER_FALLING, "switch_irq", NULL);

    if(ret){
        printk("Unable to request IRQ: %d\n",ret);
        free_irq(irq_num, NULL);
    }

    return 0;
}

static void __exit simple_tasklet_exit(void){
    tasklet_kill(&my_tasklet);
    free_irq(irq_num, NULL);
    gpio_free(SWITCH);

    printk("simple tasklet: exit module\n");
}

module_init(simple_tasklet_init);
module_exit(simple_tasklet_exit);
