#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/time.h>

#define PROC_MUTEX_NAME "lab03_mutex"
#define PROC_SPINLOCK_NAME "lab03_spinlock"

static DEFINE_MUTEX(test_mutex);
static DEFINE_SPINLOCK(test_spinlock);
static int shared_data = 0;
static struct proc_dir_entry *proc_mutex, *proc_spinlock;

static ssize_t mutex_test(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    char output[128];
    int len;
    ktime_t start, end;
    
    if (*pos > 0)
        return 0;
    
    start = ktime_get();
    
    if (mutex_lock_interruptible(&test_mutex))
        return -ERESTARTSYS;
        
    shared_data++;
    msleep(100);  // Simulate work
    
    mutex_unlock(&test_mutex);
    
    end = ktime_get();
    
    len = snprintf(output, sizeof(output),
                   "Mutex test: data=%d, time=%lld ns\n",
                   shared_data, ktime_to_ns(ktime_sub(end, start)));
    
    if (copy_to_user(buffer, output, len))
        return -EFAULT;
        
    *pos = len;
    return len;
}

static ssize_t spinlock_test(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    char output[128];
    int len;
    ktime_t start, end;
    unsigned long flags;
    
    if (*pos > 0)
        return 0;
    
    start = ktime_get();
    
    spin_lock_irqsave(&test_spinlock, flags);
    shared_data++;
    // No sleeping in spinlock!
    spin_unlock_irqrestore(&test_spinlock, flags);
    
    end = ktime_get();
    
    len = snprintf(output, sizeof(output),
                   "Spinlock test: data=%d, time=%lld ns\n",
                   shared_data, ktime_to_ns(ktime_sub(end, start)));
    
    if (copy_to_user(buffer, output, len))
        return -EFAULT;
        
    *pos = len;
    return len;
}

static const struct proc_ops mutex_fops = {
    .proc_read = mutex_test,
};

static const struct proc_ops spinlock_fops = {
    .proc_read = spinlock_test,
};

static int __init lock_init(void)
{
    proc_mutex = proc_create(PROC_MUTEX_NAME, 0444, NULL, &mutex_fops);
    proc_spinlock = proc_create(PROC_SPINLOCK_NAME, 0444, NULL, &spinlock_fops);
    
    if (!proc_mutex || !proc_spinlock) {
        if (proc_mutex) proc_remove(proc_mutex);
        if (proc_spinlock) proc_remove(proc_spinlock);
        return -ENOMEM;
    }
    
    pr_info("Lock comparison loaded\n");
    return 0;
}

static void __exit lock_exit(void)
{
    proc_remove(proc_mutex);
    proc_remove(proc_spinlock);
    pr_info("Lock comparison unloaded\n");
}

module_init(lock_init);
module_exit(lock_exit);

MODULE_LICENSE("GPL");