#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

char value[1024];

static ssize_t simple_show_value(struct device *dev, struct device_attribute *attr, char *buf){
    return snprintf(buf, PAGE_SIZE, "%s\n", value);
}

static ssize_t simple_store_value(struct device *dev, struct device_attribute *attr, const char *buf, size_t count){
    sscanf(buf, "%s", value);
    return strnlen(buf, PAGE_SIZE);
}

static DEVICE_ATTR(value, 0664, simple_show_value, simple_store_value);

static void simple_sysfs_dev_release(struct device *dev){}

static struct platform_device the_pdev = {
    .name = "simple_sysfs_device",
    .id = -1,
    .dev = {
        .release = simple_sysfs_dev_release,
    }
};


static int __init simple_sys_init(void){
    int err = 0;
    memset(value, 0, sizeof(value));

    err = platform_device_register(&the_pdev);
    if(err){
        printk("platform_device_register error");
        return err;
    }
    err = device_create_file(&the_pdev.dev, &dev_attr_value);
    if(err){
        printk("sysfs_create_file error\n");
        // goto sysfs_err;
    }

    return 0;
}

static void __exit simple_sys_exit(void){
    device_remove_file(&the_pdev.dev, &dev_attr_value);
    platform_device_unregister(&the_pdev);
}

module_init(simple_sys_init);
module_exit(simple_sys_exit);