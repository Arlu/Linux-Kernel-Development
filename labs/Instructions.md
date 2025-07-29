# Adding Lab Examples to QEMU Image
## Add to Initramfs

Copy lab files to initramfs directory:
```bash
# Assuming you're in your kernel development directory with those labs (replace x with lab's number):
mkdir -p /tmp/initramfs_new/labxx
cp labs/labxx/* /tmp/initramfs_new/labxx/
```

## Copy the current initramfs content
```bash
cd /tmp/initramfs_new
gunzip -c /path/to/initramfs_images/initramfs.cpio.gz | cpio -id
```

## Add build tools to initramfs:
```bash
# Add kernel headers and build tools if needed
mkdir -p /tmp/initramfs_new/lib/modules/$(make -C /path/to/linux kernelrelease)
cp -r /path/to/linux/include /tmp/initramfs_new/
cp -r /path/to/linux/Module.symvers /tmp/initramfs_new/
```

## Repack the initramfs:
```bash
cd /tmp/initramfs_new
find . -print0 | cpio --null -ov --format=newc | gzip -9 > /path/to/initramfs_images/initramfs_labxx.cpio.gz
```

## Boot QEMU with the new initramfs:
```bash
bashqemu-system-x86_64 \
  -kernel /path/to/linux/arch/x86/boot/bzImage \
  -initrd /path/to/initramfs_images/initramfs_labxx.cpio.gz \
  -append "console=ttyS0 nokaslr" \
  -nographic \
  -m 512M
```