KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_sol03

obj-m += netlink_kernel.o

USER_PROG = netlink_user

all: modules $(USER_PROG)

$(USER_PROG): netlink_user.c
	$(CC) -o $(USER_PROG) netlink_user.c

modules:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	rm -f $(USER_PROG)

install:
	mkdir -p $(INSTALL_PATH)/kernel_modules
	cp *.ko $(INSTALL_PATH)/kernel_modules/ 2>/dev/null || true
	mkdir -p  $(INSTALL_PATH)/lib64
	cp /lib64/ld-linux-x86-64.so.2 $(INSTALL_PATH)/lib64/
	mkdir -p $(INSTALL_PATH)/lib/x86_64-linux-gnu
	cp /lib/x86_64-linux-gnu/libc.so.6 $(INSTALL_PATH)/lib/x86_64-linux-gnu/
	mkdir -p $(INSTALL_PATH)/user_programs
	cp $(USER_PROG) $(INSTALL_PATH)/user_programs/ 2>/dev/null || true

.PHONY: all modules netlink clean install