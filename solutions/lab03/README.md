# Solution for Lab 3: Kernel Communication and Synchronization

## Part 1: debugfs

## Part 2: Netlink multicast

## Part 3: RCU

## Build and Test

### Directory Structure
```
labs/lab3/
├── Makefile
├── 01_debugfs/
│   ├── Makefile
│   └── debugfs_demo.c
├── 02_netlink_multicast/
│   ├── Makefile
│   ├── netlink_kernel.c
│   └── netlink_user.c
└── 03_rcu/
    ├── Makefile
    └── rcu_demo.c
```

### Testing Commands

```bash
# Build all modules and run:
make
make update-initramfs
make test-qemu

# Test debugfs:
mount -t debugfs none /sys/kernel/debug
insmod /kernel_modules/debugfs_demo.ko
echo 13 > /sys/kernel/debug/demo/status
cat /sys/kernel/debug/demo/status
echo "Hello debug!" > /sys/kernel/debug/demo/details
cat /sys/kernel/debug/demo/details
rmmod debugfs_demo

# Test netlink multicast:
insmod kernel_modules/netlink_kernel.ko
/user_programs/netlink_user
rmmod netlink_kernel

# Test RCU:
insmod /kernel_modules/rcu_demo.ko
echo "add First record!" > /proc/log_example
echo "add Second record!" > /proc/log_example
echo "add Third record!" > /proc/log_example
echo "get 0" > /proc/log_example
echo "get 1" > /proc/log_example
echo "get 2" > /proc/log_example
echo "del 1" > /proc/log_example
cat /proc/log_example
rmmod rcu_demo
```

## Resources

- Kernel source: `kernel/`, `fs/debugfs/`
- Synchronization primitives: `include/linux/`