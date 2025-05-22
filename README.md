# Linux-Kernel-Development

This is a complete guide, which walks through the complete process of building updated versions of a custom Linux kernel with BusyBox, creating an initramfs, and booting it using QEMU. This is useful for kernel development, testing, and learning about Linux internals.

## Prerequisites

- Ubuntu 24.04
- Git
- Basic development tools (gcc, make, etc.)
- Approximately 10GB of free disk space

Recommended to have SSH and VS Code.

I've used 16 CPU cores 16GB RAM and 64GB disk in VM on VMWare Workstation.

## 1. Install Cross Compiler

Install the cross compiler (if not already installed):

```bash
# Install development packages
sudo apt install build-essential flex bison libssl-dev libelf-dev libncurses5-dev libncursesw5-dev

# Install the cross compiler on Ubuntu/Debian
sudo apt install gcc-x86-64-linux-gnu binutils-x86-64-linux-gnu

```

## 2. Download the kernel and BusyBox source codes:

Download Linux kernels using wget:

```bash
# Latest stable for now:
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.14.7.tar.xz
# Latest LTS for now:
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.12.29.tar.xz
# LTS 6.6 (6.8+ have some problem with BusyBox for now [1.36.1 and 1.37.0])
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.91.tar.xz
# LTS 5.15:
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.183.tar.xz
```

And BusyBox:
```bash
# Latest stable for now:
wget https://busybox.net/downloads/busybox-1.36.1.tar.bz2
```

Other for using with the image:
```bash
# Create utilities directory:
mkdir utilities
cd utilities

# Init script:
wget https://github.com/Arlu/Linux-Kernel-Development/raw/refs/heads/main/_init

## Leave:
cd ..
```

## 3. Build the Linux Kernel

Navigate to the Linux source directory and build the kernel:

```bash
# Extract some version (we use 5.15):
tar -xf linux-5.15.183.tar.xz
cd linux-5.15.183
# Or with version 6.14.7:
tar -xf linux-6.14.7.tar.xz
cd linux-6.14.7

# Set the cross-compiler (if you're compiling on x86_64 for x86_64)
export CROSS_COMPILE=x86_64-linux-gnu-

# Configure the kernel for x86_64
make x86_64_defconfig

# Build the kernel (adjust the number based on your CPU cores)
make -j$(nproc)

# Verify the kernel image is created
ls arch/x86/boot/bzImage

# Make headers for using with BusyBox later (skip for 6.8 and later):
make headers_install INSTALL_HDR_PATH=~/headers/linux-5.15

# Back to home folder:
cd ..
```

## 4. Download and Build BusyBox

BusyBox provides the essential user space utilities:

```bash
# Extract some version:
tar -xf busybox-1.36.1.tar.bz2
cd busybox-1.36.1

# Install required ncurses libraries if missing
# sudo apt-get install libncurses5-dev libncursesw5-dev

# Create a default configuration
make defconfig

# Configure BusyBox to build statically
make menuconfig
# In the menuconfig interface:
# Navigate to "Settings" → Find "Build static binary (no shared libs)"
# Press 'Y' to enable it
# (For 6.7 and earlier)
# Then navigate to "() Additional CFLAGS"
# Press enter, then add: -I/home/arie/headers/linux-5.15/include/
# (For 6.8 and later)
# Just disable some, press esc, then navigate to "Networking Utilities" → Find "tc (8.3 kb)"
# Press 'N' to disable it (because it isn't support by these kernels for now)
# Exit and save the configuration

# Build BusyBox
make -j$(nproc)

# Install BusyBox
make install

# Back to home folder:
cd ..
```

## 5. Create an Initramfs

Now create a minimal filesystem for the kernel to boot:

```bash
# Create a dedicated directory
mkdir -p /tmp/initramfs
cd /tmp/initramfs

# Create the basic directory structure
mkdir -p {bin,sbin,etc,proc,sys,dev,usr/{bin,sbin},root}

# Copy the BusyBox binary:
cp ~/busybox-1.36.1/busybox bin/

# Create essential symlinks
cd bin
ln -s busybox sh
cd ..

# Create a proper init script with explicit BusyBox paths
cp ~/utilities/_init ./init

# Make init executable
chmod +x init

# Create basic configuration files
cat > etc/passwd << EOF
root::0:0:root:/root:/bin/sh
EOF

cat > etc/group << EOF
root:x:0:
EOF
```

## 5. Package the Initramfs

Create the compressed initramfs image:

```bash
# Create images folder:
mkdir ~/images

# Package the initramfs with a clear filename to avoid confusion
find . -print0 | cpio --null -ov --format=newc | gzip -9 > ~/images/initramfs.cpio.gz

# Return to the project directory
cd ~/images/
```

## 6. Boot with QEMU

Finally, boot your custom Linux kernel with the initramfs using QEMU:

```bash
# Installation
sudo apt install virt-manager
Or
sudo apt-get install qemu-system-x86 qemu-utils

# Start QEMU with your kernel and initramfs
cd ~/images/
qemu-system-x86_64 -kernel ~/linux-5.15.183/arch/x86/boot/bzImage -initrd initramfs.cpio.gz -append "console=ttyS0 nokaslr" -nographic -m 512M
# Or:
qemu-system-x86_64 -kernel ~/linux-6.14.7/arch/x86/boot/bzImage -initrd initramfs.cpio.gz -append "console=ttyS0 nokaslr" -nographic -m 512M

```

## 7. Working with Your Custom Linux System

### Basic Usage
After booting, you'll see a shell prompt (`/ #`). You can now use the various BusyBox commands.

### Exiting QEMU
To exit the QEMU session:
- Press `Ctrl+A` followed by `x`