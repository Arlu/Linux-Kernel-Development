# Setup
export INSTALL_PATH=/tmp/initramfs_new
export INITRAMFS_ORIG=~/images/initramfs_libc.cpio.gz
export LAB_DIR=$PWD
export FS_NAME=sol02

# Extract and prepare
rm -rf $INSTALL_PATH
mkdir -p $INSTALL_PATH
cd $INSTALL_PATH && gunzip -c $INITRAMFS_ORIG | cpio -id

# Copy files
mkdir -p lib/modules bin
cp $LAB_DIR/test_hello bin/
cp $LAB_DIR/test_calc bin/
cp $LAB_DIR/test_reverse_string bin/
chmod +x bin/*

# Package
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs_$FS_NAME.cpio.gz