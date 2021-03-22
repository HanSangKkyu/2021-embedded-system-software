#include <linux/init.h>
#include <linux/module.h>

#ifdef MY_DEBUG
	#define DEBUG_MSG(fmt, args...) \
		printk(KERN_DEBUG "MYDEBUG: " fmt, ##args);
#else
	#define DEBUG_MSG(fmt, args...)
#endif

int my_id = 20207777;
EXPORT_SYMBOL(my_id);

static int __init my_init(void)
{
	printk(KERN_NOTICE "Hello, Mod1\n");
	return 0;
}


static void __exit my_exit(void)
{
	printk(KERN_NOTICE "Goodbye, Mod1\n");
}

module_init(my_init);
module_exit(my_exit);
