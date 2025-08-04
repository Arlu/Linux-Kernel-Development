KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_lab03

obj-m += procfs_basic.o
obj-m += sysfs_demo.o

all: modules netlink

modules:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

netlink:
	$(MAKE) -C netlink_demo

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	$(MAKE) -C netlink_demo clean

install:
	mkdir -p $(INSTALL_PATH)/kernel_modules
	cp *.ko $(INSTALL_PATH)/kernel_modules/ 2>/dev/null || true
	$(MAKE) -C netlink_demo install INSTALL_PATH=$(INSTALL_PATH)

.PHONY: all modules netlink clean install