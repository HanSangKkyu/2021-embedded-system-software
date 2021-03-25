#define IOCTL_START_NUM Ox80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2
#define SIMPLE_IOCTL_NUM 'z'
#define SIMPLE_SPIN_READ_IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define SIMPLE_SPIN_WRITE_IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unstgned long *)
struct str_st {
	int len; 
	char str [128];
};