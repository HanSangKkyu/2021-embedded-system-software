#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define MAX_TIMING 85
#define DHT11 21

static int dht11_data[5] = {0,};

static void dht11_read(void){
    int last_state = 1;
    int counter = 0;
    int i =0, j=0;

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
    }else{
        printk("Data not good, skip\n");
    }
}

static int __init simple_dht11_init(void){
    int i = 0;

    gpio_request(DHT11, "DHT11");

    for(i = 0;i<10;i++){
        dht11_read();
        mdelay(1000);
    }

    return 0;

}

static void __exit simple_dht11_exit(void){
    gpio_set_value(DHT11, 0);
    gpio_free(DHT11);
}

module_init(simple_dht11_init);
module_exit(simple_dht11_exit);
