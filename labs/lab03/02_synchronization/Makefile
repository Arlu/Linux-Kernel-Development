KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_lab03

obj-m += atomic_demo.o
obj-m += lock_comparison.o
obj-m += rwlock_demo.o

all: modules

modules:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean

install:
	mkdir -p $(INSTALL_PATH)/kernel_modules
	cp *.ko $(INSTALL_PATH)/kernel_modules/ 2>/dev/null || true

.PHONY: all modules netlink clean install