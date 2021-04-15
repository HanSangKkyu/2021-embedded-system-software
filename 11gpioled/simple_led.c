
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

#define LED1 4

static int __init simple_led_init(void){
    printk("Hello LED\n");
    gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
    gpio_set_value(LED1, 1);

    return 0;
}

static void __exit simple_led_exit(void){
    printk("Bye LED \n");

    gpio_set_value(LED1, 0);

    gpio_free(LED1);
}

module_init(simple_led_init);
module_exit(simple_led_exit);