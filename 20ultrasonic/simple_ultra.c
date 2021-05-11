#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <asm/delay.h>


MODULE_LICENSE("GPL");

#define ULTRA_TRIG 17
#define ULTRA_ECHO 18

static int irq_num;
static int echo_valid_flag = 3;

static ktime_t echo_start;
static ktime_t echo_stop;

static irqreturn_t simple_ultra_isr(int irq, void* dev_id){
    ktime_t tmp_time;
    s64 time;
    int cm;
    
    tmp_time = ktime_get();
    if(echo_valid_flag == 1){

        printk("simple_ultra : Echo UP\n");
        if(gpio_get_value(ULTRA_ECHO) == 1){
            echo_start = tmp_time;
            echo_valid_flag = 2;
        }
    } else if(echo_valid_flag == 2){
        printk("simple_ultra : Echo down\n");
        if(gpio_get_value(ULTRA_ECHO) == 0){
            echo_stop = tmp_time;
            time = ktime_to_us(ktime_sub(echo_stop, echo_start));
            cm = (int) time / 58;
            printk("simple_ultra : detect %d cm\n",cm);

            echo_valid_flag = 3;
        }
    }
    return IRQ_HANDLED;
}

static int __init simple_ultra_init(void){
    int ret;
    
    printk("simple_ultra : init module\n");

    gpio_request_one(ULTRA_TRIG, GPIOF_OUT_INIT_LOW, "ULTRA_TRIG");
    gpio_request_one(ULTRA_ECHO, GPIOF_IN, "ULTRA_ECHO");

    irq_num = gpio_to_irq(ULTRA_ECHO);
    ret = request_irq(irq_num, simple_ultra_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "ULTRA_ECHO", NULL);
    if(ret){
        printk("simple_ultra : unable to request IRQ : %d\n", ret);
        free_irq(irq_num, NULL);
    }else{
        if(echo_valid_flag == 3){
            echo_start = ktime_set(0,1);
            echo_stop = ktime_set(0,1);
            echo_valid_flag = 0;

            gpio_set_value(ULTRA_TRIG, 1);
            udelay(10);
            gpio_set_value(ULTRA_TRIG, 0);

            echo_valid_flag = 1;
        }
    }

    return 0;
}

static void __exit simple_ultra_exit(void){
    printk("simple_ultra: exit modules \n");
    free_irq(irq_num, NULL);
}

module_init(simple_ultra_init);
module_exit(simple_ultra_exit);
