#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2
#define IOCTL_NUM3 IOCTL_START_NUM + 3
#define IOCTL_NUM4 IOCTL_START_NUM + 4

#define SIMPLE_IOCTL_NUM 'z'
#define WQ _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long)
#define WQ_INTERRUPTIBLE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long)
#define WQ_INTERRUPTIBLE_TIMEOUT _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM3, unsigned long)
#define WQ_WAKE_UP _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM4, unsigned long)