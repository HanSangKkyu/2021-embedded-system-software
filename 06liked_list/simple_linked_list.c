#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");

struct test_list{
	struct list_head list;
	int id;
};

struct test_list mylist;

static int __init simple_init(void)
{
	struct test_list *tmp = 0;
	struct list_head *pos = 0;
	unsigned lnt i;

	printk("simple_linked_list: init module\n");
	
	INIT_LIST_HEAD(&mylist.list);

	for(i=0;i<5;i++){
		tmp = (struct test_list*)kmalloc(sizeof(struct test_list), GFP_KERNEL);
		tmp->id = i;
		printk("simple_linked_list: enter to list [%d]\n", tmp->id);
		list_add(&tmp->list, &mylist.list);
	}

	i = 0;

	printk("simple_linked_list: use list_for_each & list_entry\n");
	list_for_each(pos, &mylist.list){
		tmp = list_entry(pos, struct test_list, list);
		printk("simple_linked_list: pos[%d], id[%d] \n",i,tmp->id);
		i ++;
	}
		
	return 0;
}


static void __exit simple_exit(void)
{
	struct test_list *tmp = 0;
	struct list_head *pos = 0;
	struct list_head *q = 0;
	unsigned int i = 0;

	printk("simple_linked_list: exit module\n");
	i = 0;
	list_for_each_safe(pos, q, &mylist.list){
		tmp = list_entry(pos, struct test_list, list);
		printk("simple_linked_list: free pos[%d], id[%d]", i , tmp->id);
		kfree(tmp);
		i++;
	}
  	
}

module_init(simple_init);
module_exit(simple_exit);
