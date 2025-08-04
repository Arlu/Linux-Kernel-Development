KERNEL_SRC=~/linux-6.16
KDIR ?= $(KERNEL_SRC)
INSTALL_PATH ?= /tmp/initramfs_lab03
INITRAMFS_ORIG=~/images/initramfs.cpio.gz

SUBDIRS = 01_communication 02_synchronization

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ KDIR=$(KDIR)

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

install: all
	mkdir -p $(INSTALL_PATH)/kernel_modules
	mkdir -p $(INSTALL_PATH)/user_programs
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install INSTALL_PATH=$(INSTALL_PATH); \
	done

update-initramfs: install
	@echo "Extracting original initramfs..."
	mkdir -p $(INSTALL_PATH)
	cd $(INSTALL_PATH) && gunzip -c $(INITRAMFS_ORIG) | cpio -id
	
	@echo "Creating new initramfs..."
	cd $(INSTALL_PATH) && find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs_lab03.cpio.gz
	
	@echo "New initramfs: ~/images/initramfs_lab03.cpio.gz"

test-qemu:
	qemu-system-x86_64 \
		-kernel $(KDIR)/arch/x86/boot/bzImage \
		-initrd ~/images/initramfs_lab03.cpio.gz \
		-append "console=ttyS0 nokaslr" \
		-nographic \
		-m 512M

.PHONY: all clean install test-qemu $(SUBDIRS)