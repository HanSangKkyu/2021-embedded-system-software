#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/delay.h>


MODULE_LICENSE("GPL");

#define SPEAKER 12

static void play(int note){
    int i = 0;
    for(i = 0;i<100;i++){
        gpio_set_value(SPEAKER, 1);
        udelay(note);
        gpio_set_value(SPEAKER, 0);
        udelay(note);
    }
}

static int __init simple_speaker_init(void){
    int notes[] = {1911, 1702, 1516, 1431, 1275, 1136, 1012};
    int i = 0;

    gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW, "SPEAKER");

    for(i=0;i<7;i++){
        play(notes[i]);
        mdelay(500);
    }

    gpio_set_value(SPEAKER, 0);
    return 0;
}

static void __exit simple_speaker_exit(void){
    gpio_set_value(SPEAKER, 0);
    gpio_free(SPEAKER);
}

module_init(simple_speaker_init);
module_exit(simple_speaker_exit);
