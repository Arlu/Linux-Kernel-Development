#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>

#define DEBUG_DIR "demo"
#define DEBUG_CONTENT "details"
#define DEBUG_STATUS "status"
#define DETAILS_MAX_SIZE 128

struct dentry *debug_dir, *debug_content;

static u64 status;
static char details[DETAILS_MAX_SIZE];
static int len;

static ssize_t read_details(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    if (*pos > 0)
        return 0;
    
    if (copy_to_user(buf, details, len))
        return -EFAULT;
    
    *pos = len;
    return len;
}

static ssize_t write_details(struct file *file, const char __user *buf, size_t count, loff_t *pos)
{
    if (count > DETAILS_MAX_SIZE - 1)
        return -EINVAL;
        
    if (copy_from_user(details, buf, count))
        return -EFAULT;
        
    details[count] = '\0';
    len = count + 1;
    
    return count;
}

static const struct file_operations debug_fops = {
    .read = read_details,
    .write = write_details
};

static int __init debugfs_example_init(void)
{
    debug_dir = debugfs_create_dir(DEBUG_DIR, NULL);
    if (!debug_dir)
        return -ENOMEM;
    
    debug_content = debugfs_create_file(DEBUG_CONTENT, 0666, debug_dir, NULL, &debug_fops);
    debugfs_create_x64(DEBUG_STATUS, 0666, debug_dir, &status);

    pr_info("debugfs module loaded /sys/kernel/debug/%s\n", DEBUG_DIR);
    return 0;
}

static void __exit debugfs_example_exit(void)
{
    debugfs_remove(debug_dir);
    pr_info("debugfs module unloaded\n");
}

module_init(debugfs_example_init);
module_exit(debugfs_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arie");
MODULE_DESCRIPTION("A example of debugfs module");
MODULE_VERSION("1.0");