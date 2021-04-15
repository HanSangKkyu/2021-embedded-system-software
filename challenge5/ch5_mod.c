#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define LED1 5
#define SWITCH 12

static int count = 0;

static int __init ch5_init(void){
    int ret = 0;
    printk("ch5 : init module \n");
    gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
    gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");

    while(true){
        ret = gpio_get_value(SWITCH); // if pushing button, get 1 else get 0
        printk("ret = %d\n", ret);
        if(ret){
            gpio_set_value(LED1, 1);
            printk("ch5: pushed button, count = %d", count);
        }else{
            gpio_set_value(LED1, 0);
        }

        msleep(500);
    }

    return 0;
}

static void __exit ch5_exit(void){
    printk("Bye ch5 \n");

    gpio_free(LED1);
    gpio_free(SWITCH);
}

module_init(ch5_init);
module_exit(ch5_exit);
