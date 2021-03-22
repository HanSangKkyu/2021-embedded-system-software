#include <linux/init.h>
#include <linux/module.h>


#ifdef MY_DEBUG
	#define DEBUG_MSG(fmt, args...) \
		printk(KERN_DEBUG "MYDEBUG: " fmt, ##args);
#else
	#define DEBUG_MSG(fmt, args...)
#endif


extern int my_id;

static int __init my_init(void)
{
	printk(KERN_NOTICE "Hello Mod2 \n");
	printk(KERN_NOTICE "My Id : %d\n", my_id);
	return 0;
}


static void __exit my_exit(void)
{
	printk(KERN_NOTICE "goodbye Mod2\n");
}

module_init(my_init);
module_exit(my_exit);
