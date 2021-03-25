#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2
#define SIMPLE_IOCTL_NUM 'z'
#define CH3_IOCTL_READ _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define CH3_IOCTL_WRITE _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
struct msg_st {
	int len; 
	char str [128];
};