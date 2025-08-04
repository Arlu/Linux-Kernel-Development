#include <linux/module.h>
#include <linux/proc_fs.h>

#define PROC_NAME "lab03_basic"
#define MAX_SIZE 1024

static struct proc_dir_entry *proc_entry;
static char kernel_buffer[MAX_SIZE];

static ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    int len;
    
    if (*pos > 0)
        return 0;
        
    len = snprintf(kernel_buffer, MAX_SIZE, "Hello from procfs! PID: %d\n", current->pid);
    
    if (copy_to_user(buffer, kernel_buffer, len))
        return -EFAULT;
        
    *pos = len;
    return len;
}

static ssize_t proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    if (count > MAX_SIZE - 1)
        return -EINVAL;
        
    if (copy_from_user(kernel_buffer, buffer, count))
        return -EFAULT;
        
    kernel_buffer[count] = '\0';
    pr_info("Received: %s", kernel_buffer);
    
    return count;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init procfs_init(void)
{
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry)
        return -ENOMEM;
        
    pr_info("procfs module loaded: /proc/%s\n", PROC_NAME);
    return 0;
}

static void __exit procfs_exit(void)
{
    proc_remove(proc_entry);
    pr_info("procfs module unloaded\n");
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");