CC = x86_64-linux-gnu-gcc
export CROSS_COMPILE=x86_64-linux-gnu-

KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_new
INITRAMFS_ORIG=~/images/initramfs.cpio.gz


obj-m += spinlock_demo.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	mkdir -p $(INSTALL_PATH)/lib/modules
	cp *.ko $(INSTALL_PATH)/lib/modules

update-initramfs: install
	@echo "Extracting original initramfs..."
	mkdir -p $(INSTALL_PATH)
	cd $(INSTALL_PATH) && gunzip -c $(INITRAMFS_ORIG) | cpio -id
	
	@echo "Copying kernel modules..."
	mkdir -p $(INSTALL_PATH)/lib/modules
	cp *.ko $(INSTALL_PATH)/lib/modules
	
	@echo "Creating new initramfs..."
	cd $(INSTALL_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs_class3.cpio.gz
	
	@echo "New initramfs: ~/images/initramfs_class3.cpio.gz"

# Test with QEMU
test-qemu:
	qemu-system-x86_64 \
		-kernel $(KDIR)/arch/x86/boot/bzImage \
		-initrd ~/images/initramfs_class3.cpio.gz \
		-append "console=ttyS0 nokaslr" \
		-nographic \
		-m 512M

test:
	@echo "Testing ..."
	# insmod lib/modules/spinlock_demo.ko

	# # Read from procfs interface (triggers spinlock usage)
	# cat /proc/spinlock_demo

	# # Write to procfs interface (triggers spinlock usage)
	# echo 5 > /proc/spinlock_demo

	# # Test concurrent access
	# cat /proc/spinlock_demo &
	# cat /proc/spinlock_demo &
	# cat /proc/spinlock_demo &
	# 
	@echo " read test complete"

	# # Test concurrent access
	# echo 6 > /proc/spinlock_demo &
	# echo 7 > /proc/spinlock_demo &
	# echo 9 > /proc/spinlock_demo &
	# 
	@echo " write test complete"

.PHONY: all clean install test