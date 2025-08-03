CC = x86_64-linux-gnu-gcc
export CROSS_COMPILE=x86_64-linux-gnu-

KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_new
INITRAMFS_ORIG=~/images/initramfs.cpio.gz


obj-m += sysfs_example.o

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
	@echo "Testing advanced procfs..."
	# Load module
	insmod sysfs_example.ko

	# Check created interface
	ls /sys/kernel/kernel_course/

	# Read attributes
	cat /sys/kernel/kernel_course/value
	cat /sys/kernel/kernel_course/string

	# Write attributes
	echo 42 > /sys/kernel/kernel_course/value
	echo "Hello sysfs" > /sys/kernel/kernel_course/string

	# Verify changes
	cat /sys/kernel/kernel_course/value
	cat /sys/kernel/kernel_course/string
	@echo "Advanced procfs test complete"

.PHONY: all clean install test