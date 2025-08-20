CC = x86_64-linux-gnu-gcc
export CROSS_COMPILE=x86_64-linux-gnu-

KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_new
INITRAMFS_ORIG=~/images/initramfs.cpio.gz

obj-m += sync_lab.o
obj-m += data_lab.o  
obj-m += irq_change_lab.o

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
	cd $(INSTALL_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs_lab04.cpio.gz
	
	@echo "New initramfs: ~/images/initramfs_lab04.cpio.gz"

# Test with QEMU
test-qemu:
	qemu-system-x86_64 \
		-kernel $(KDIR)/arch/x86/boot/bzImage \
		-initrd ~/images/initramfs_lab04.cpio.gz \
		-append "console=ttyS0 nokaslr" \
		-nographic \
		-m 512M

test: all
	@echo "=== Testing Lab Exercises ==="
	
# 	@echo "Loading modules..."
# 	sudo insmod sync_lab.ko
# 	sudo insmod data_lab.ko  
# 	sudo insmod irq_change_lab.ko
	
	@echo "Testing synchronization..."
	insmod /lib/modules/sync_lab.ko
	cat /proc/sync_lab
	echo 150 > /proc/sync_lab
	sleep 2
	cat /proc/sync_lab
	rmmod sync_lab
	
	@echo "Testing data structures..."
	insmod /lib/modules/data_lab.ko
	echo "task 1 high_priority 10" > /proc/data_lab
	echo "user 1001 alice 0" > /proc/data_lab
	echo "msg 1 hello_world 0" > /proc/data_lab
	cat /proc/data_lab
	rmmod data_lab
	
	@echo "Testing interrupt changes..."
	insmod /lib/modules/irq_change_lab.ko
	cat /proc/irq_changes
	rmmod irq_change_lab
	
# 	@echo "Cleaning up..."
# 	sudo rmmod irq_change_lab
# 	sudo rmmod data_lab
# 	sudo rmmod sync_lab
	
	@echo "Lab test complete!"

.PHONY: all clean install test