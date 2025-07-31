export FS_NAME=$1

# Test in QEMU
qemu-system-x86_64 \
  -kernel ~/images/bzImage \
  -initrd ~/images/initramfs_$FS_NAME.cpio.gz \
  -append "console=ttyS0 nokaslr" \
  -nographic -m 512M