#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/rculist.h>


MODULE_LICENSE("GPL");

#define DEV_NAME "simple_rculist_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_UPDATE _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

struct test_list{
    struct list_head list;
    unsigned long id;
    unsigned long data;
};
spinlock_t my_lock;

static dev_t dev_num;
static struct cdev *cd_cdev;
struct test_list __rcu my_list; //  shared rcu list

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

    struct test_list *pos;
    struct test_list *new;
    struct test_list *old;
    unsigned long flags;

	switch(cmd){
        case IOCTL_READ:
            rcu_read_lock();
            printk("simple_rculist : Read");
            list_for_each_entry_rcu(pos,&mylist.list, list){
                if(pos->id == arg){
                    printk("simple_rculist : List [id = %ld] = %ld\n", arg, pos->data);
                    rcu_read_unlock();
                    return 0;
                }
            }
            
			printk("simple_rculist : Read Not Found Id =  %ld\n", arg);
            rcu_read_unlock();
            break;
		case IOCTL_UPDATE:
            spin_lock_irqsave(&my_lock, flags);
            printk("simple_rculist : Update \n");
            list_for_each_entry_rcu(pos, &mylist.list, list){
                if(pos->id == arg){
                    old = pos;
                    new (struct test_list*)kmalloc(sizeof(struct test_list), GFP_KERNEL);
                    memcpy(new, old, sizeof(struct test_list));
                    new->data++;
                    list_replace_rcu(&pos->list, &new->list);
                    synchronize_rcu();
                    kfree(old);
                    printk("simple_rculist : List [id = %ld] = %ld\n", arg, new->data);
                    spin_unlock_irqrestore(&my_lock, flags);
                    return 0;
                }
            }
			printk("simple_rculist : Update Not Found Id =  %ld\n", arg);
            spin_unlock_irqrestore(&my_lock, flags);
			break;
        default:
            return -1;
	}

	return 0;
}

static int simple_rculist_open(struct inode *inode, struct file *file){
    printk("simple_rculist : open \n");
    return 0;
}

static int simple_rculist_release(struct inode *inode, struct file *file){
    printk("simple_rculist : close \n");
    return 0;
}

struct file_operations simple_rculist_fops = {
    .unlocked_ioctl = simple_ioctl,
    .open = simple_rculist_open,
    .release = simple_rculist_release,
};

static int __init simple_rculist_init(void){
    int i;
    struct test_list *pos;
    
    printk("simple_rculist: init modules \n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_rculist_fops);
    cdev_add(cd_cdev, dev_num, 1);

    spin_lock_init(&my_lock);

    INIT_LIST_HEAD(&mylist.list);

    for(i=0;i<5;i++){
        pos = (struct test_list*)kmalloc(sizeof(struct test_list),GFP_KERNEL);
        pos->id = i+1;
        pos->data = 100;
        list_add_tail_rcu(&(pos->list), &(mylist.list));
    }

    printk("simple_rculist : init module\n");

    return 0;
}

static void __exit simple_rculist_exit(void){
    struct test_list *pos;
    struct test_list *tmp;
    unsigned long flags;

    printk("simple_rculist: exit modules \n");

    spin_lock_irqsave(&my_lock,flags);

    list_for_each_entry_safe(tmp, pos, &mylist.list, list){
        list_del_rcu(&(tmp->list));
        synchronize_rcu();
        kfree(tmp);
    }

    spin_unlock_irqrestore(&my_lock, flags);

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(simple_rculist_init);
module_exit(simple_rculist_exit);
