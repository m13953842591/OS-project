ifneq ($(KERNELRELEASE),)
mymodule-objs := taskinfo.o
obj-m := taskinfo.o
 
else
PWD := $(shell pwd)
KVER ?= $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build
 
all:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	rm -rf .*.cmd *.o *.mod.c *.ko .tmp_versions Module.symvers modules.order
endif