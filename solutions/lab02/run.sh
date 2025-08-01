FS_NAME=sol02
KRNL_NAME=sol02

# Test in QEMU
qemu-system-x86_64 \
  -kernel ~/images/vmlinux-$KRNL_NAME \
  -initrd ~/images/initramfs_$FS_NAME.cpio.gz \
  -append "console=ttyS0 nokaslr" \
  -nographic -m 512M