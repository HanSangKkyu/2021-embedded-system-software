#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define READ 1
#define UPDATE 2

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM + 1
#define IOCTL_NUM2 IOCTL_START_NUM + 2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_UPDATE _IOWR (SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

int main(int argc, char *argv[]){
    int dev;
    int i, n, op;

    if(argc != 3){ // ./simple_rculist 1 10
        printf("Insert two arguments \n");
        printf("First argument = (1:read, 2:update) \n");
        printf("Second argument = id \n");
        return -1;
    }

    op = atoi(argv[1]);
    n = atoi(argv[2]);

    dev = open("/dev/simple_rculist_dev", O_RDWR);

    if(op ==READ){
        ioctl(dev, IOCTL_READ, (unsigned long) id);
    }else if(op == UPDATE){
        ioctl(dev, IOCTL_UPDATE, (unsigned long) id);
    }else{
        printf("Invalid Operation\n");
        close(dev);
        return -1;
    }

    close(dev);
}