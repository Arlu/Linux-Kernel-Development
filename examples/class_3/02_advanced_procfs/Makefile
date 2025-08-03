CC = x86_64-linux-gnu-gcc

export CROSS_COMPILE=x86_64-linux-gnu-

KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_new
INITRAMFS_ORIG=~/images/initramfs.cpio.gz

obj-m += pos_demo.o

# Build kernel modules
modules:
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
	# # Load the module
	# insmod /lib/modules/pos_demo.ko
	# # Add log entries
	# echo "First log entry" > /proc/virtual_log
	# echo "Second log entry" > /proc/virtual_log
	# echo "Third log entry" > /proc/virtual_log
	# echo "Fourth log entry" > /proc/virtual_log
	# # Read all entries (position starts at 0)
	# cat /proc/virtual_log
	# # Verify interface created
	# ls -la /proc/virtual_log
	# dmesg | tail -5
	# # # Remove the module
 	# # rmmod pos_demo


.PHONY: modules clean install remove test-all