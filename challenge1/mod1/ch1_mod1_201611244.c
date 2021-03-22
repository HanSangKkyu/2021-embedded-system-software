#include <linux/init.h>
#include <linux/module.h>

#ifdef MY_DEBUG
	#define DEBUG_MSG(fmt, args...) \
		printk(KERN_DEBUG "MYDEBUG: " fmt, ##args);
#else
	#define DEBUG_MSG(fmt, args...)
#endif

int my_id = 20207777;

static int get_my_id(void)
{
	return my_id;
}

static int set_my_id(int id)
{
	my_id = id;
	return 0;	
}

EXPORT_SYMBOL(get_my_id);
EXPORT_SYMBOL(set_my_id);

static int __init my_init(void)
{
	return 0;
}


static void __exit my_exit(void)
{
	
}

module_init(my_init);
module_exit(my_exit);
