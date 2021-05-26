obj-m := ku_ipc.o

KERNELDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(ARM) clean
scp:
	scp $(obj-m:.o=.ko) $(obj-m:.o=_lib.c) $(obj-m:.o=_app.c) mknod.sh pi@10.10.10.12:~/
