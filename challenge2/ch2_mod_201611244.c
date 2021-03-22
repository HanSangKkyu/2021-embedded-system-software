#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "ch2_dev"

#define IOCTL_START_NUM 0x80 // 매직넘버 다른 값을 쓰는게 좋다
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define PARAM_IOCTL_NUM 'z'
#define GET_IOCTL _IOWR(PARAM_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define SET_IOCTL _IOWR(PARAM_IOCTL_NUM, IOCTL_NUM2, unsigned long)
#define ADD_IOCTL _IOWR(PARAM_IOCTL_NUM, IOCTL_NUM3, unsigned long)
#define MUL_IOCTL _IOWR(PARAM_IOCTL_NUM, IOCTL_NUM4, unsigned long)


static long result = 0;
module_param(result, long, 0);

static int simple_param_open(struct inode *inode, struct file *file){
	printk("simple_param: open\n");
	return 0;
}

static int simple_param_release(struct inode *inode, struct file *file){
	printk("simple_param: release\n");
	return 0;
}

static long simple_param_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	switch(cmd){
		case GET_IOCTL:
			// printk("simple_param: return result %lu\n", result);
			return result;
		case SET_IOCTL:
			// printk("simple_param: set result %ld to %ld \n", result, (long)arg);
			result = (long) arg;
			return result;
		case ADD_IOCTL:
			result = result + (long) arg;
			return result;
		case MUL_IOCTL:
			result = result * (long) arg;
			return result;
		default:
			return -1;
	}

	return 0;
}

struct file_operations simple_param_fops = {
	.open = simple_param_open,
	.release = simple_param_release,
	.unlocked_ioctl = simple_param_ioctl, // ioctl 자리
  // 더 많은 함수들이 존재하지만 필요한 부분만 초기화 해주면 된다.
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init simple_param_init(void)
{
	printk("simple_param: init module\n");
	
	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME); // major넘버를 얻어온다
	cd_cdev = cdev_alloc();// cdev구조체를 초기화 한다.
	cdev_init(cd_cdev, &simple_param_fops);// cdev에 file operation을 할당한다.
	cdev_add(cd_cdev, dev_num, 1); // cdev를 디바이스 드라이버에 넣는다.
  
	return 0;
}


static void __exit simple_param_exit(void)
{
	printk("simple_param: exit module\n");
  	
	cdev_del(cd_cdev); // cdev를 삭제한다 
	unregister_chrdev_region(dev_num, 1); // major 넘버를 삭제한다
}

module_init(simple_param_init);
module_exit(simple_param_exit);
