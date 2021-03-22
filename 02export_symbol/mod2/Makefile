obj-m := mod2.o

KEBUILD_EXTRA_SYMBOLS := /home/han/Documents/embeded/02export_symbol/mod2/Module.symvers
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
debug:
	$(MAKE) EXTRA_CFLAGS=-DMY_DEBUG -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
