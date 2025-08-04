#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/proc_fs.h>

#define PROC_NAME "lab03_atomic"

static atomic_t counter = ATOMIC_INIT(0);
static atomic_t operations = ATOMIC_INIT(0);
static unsigned long flags = 0;

#define FLAG_READY 0
#define FLAG_BUSY  1

static struct proc_dir_entry *proc_atomic;

static ssize_t do_atomic_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    char input[32];
    char cmd[16];
    int value;
    
    if (count > sizeof(input) - 1)
        return -EINVAL;
        
    if (copy_from_user(input, buffer, count))
        return -EFAULT;
        
    input[count] = '\0';
    
    if (sscanf(input, "%15s %d", cmd, &value) < 1)
        return -EINVAL;
    
    atomic_inc(&operations);
    
    if (strcmp(cmd, "inc") == 0)
    {
        atomic_inc(&counter);
        set_bit(FLAG_BUSY, &flags);
    }
    else if (strcmp(cmd, "dec") == 0)
    {
        atomic_dec(&counter);
    }
    else if (strcmp(cmd, "add") == 0)
    {
        atomic_add(value, &counter);
    }
    else if (strcmp(cmd, "set") == 0)
    {
        atomic_set(&counter, value);
        clear_bit(FLAG_BUSY, &flags);
    }
    
    return count;
}

static ssize_t do_atomic_read(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    char output[256];
    int len;
    
    if (*pos > 0)
        return 0;
    
    len = snprintf(output, sizeof(output),
                   "Counter: %d\nOperations: %d\nFlags: 0x%lx\n"
                   "Ready: %s\nBusy: %s\n",
                   atomic_read(&counter),
                   atomic_read(&operations),
                   flags,
                   test_bit(FLAG_READY, &flags) ? "YES" : "NO",
                   test_bit(FLAG_BUSY, &flags) ? "YES" : "NO");
    
    if (copy_to_user(buffer, output, len))
        return -EFAULT;
        
    *pos = len;
    return len;
}

static const struct proc_ops atomic_fops = {
    .proc_read = do_atomic_read,
    .proc_write = do_atomic_write,
};

static int __init atomic_init(void)
{
    set_bit(FLAG_READY, &flags);

    proc_atomic = proc_create(PROC_NAME, 0666, NULL, &atomic_fops);
    
    if (!proc_atomic)
        return -ENOMEM;
        
    pr_info("Atomic demo loaded\n");
    return 0;
}

static void __exit atomic_exit(void)
{
    proc_remove(proc_atomic);
    pr_info("Final counter: %d, operations: %d\n",
            atomic_read(&counter), atomic_read(&operations));
}

module_init(atomic_init);
module_exit(atomic_exit);

MODULE_LICENSE("GPL");