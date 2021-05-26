obj-m := ku.o

KERNELDIR := ~/linux-rpi/
ARM := ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi-
PWD := $(shell pwd)

scp:
	scp $(obj-m:.o=_sense.c) $(obj-m:.o=_act.c) $(obj-m:.o=_app.c) pi@10.10.10.12:~/

