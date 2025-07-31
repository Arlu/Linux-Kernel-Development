CC = x86_64-linux-gnu-gcc

KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_new
INITRAMFS_ORIG=~/images/initramfs.cpio.gz

obj-m += inline_assembly.o

# Build kernel modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install:
	mkdir -p $(INSTALL_PATH)/lib/modules
	cp *.ko $(INSTALL_PATH)/lib/modules

update-initramfs: #install
	@echo "Extracting original initramfs..."
	rm -rf $(INSTALL_PATH)
	mkdir -p $(INSTALL_PATH)
	cd $(INSTALL_PATH) && gunzip -c $(INITRAMFS_ORIG) | cpio -id
	
	@echo "Copying kernel modules..."
	mkdir -p $(INSTALL_PATH)/lib/modules
	cp *.ko $(INSTALL_PATH)/lib/modules
	
	@echo "Creating new initramfs..."
	cd $(INSTALL_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs_inline_assembly.cpio.gz
	
	@echo "New initramfs: ~/images/initramfs_inline_assembly.cpio.gz"

# Test with QEMU
test-qemu:
	qemu-system-x86_64 \
		-kernel $(KDIR)/arch/x86/boot/bzImage \
		-initrd ~/images/initramfs_inline_assembly.cpio.gz \
		-append "console=ttyS0 nokaslr" \
		-nographic \
		-m 512M

test:
	insmod inline_assembly.ko example=<0-7>

.PHONY: modules clean install remove test-all