#include <linux/module.h>
#include <linux/rwlock.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>

#define PROC_READER_NAME "lab03_reader"
#define PROC_WRITER_NAME "lab03_writer"

static DEFINE_RWLOCK(shared_rwlock);
static int shared_data = 0;
static unsigned long last_update = 0;
static struct proc_dir_entry *proc_reader, *proc_writer;

static ssize_t reader_proc(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    char output[128];
    int len, data;
    unsigned long update_time;
    
    if (*pos > 0)
        return 0;
    
    read_lock(&shared_rwlock);
    data = shared_data;
    update_time = last_update;
    read_unlock(&shared_rwlock);
    
    len = snprintf(output, sizeof(output), "Read data: %d (updated at %lu)\n", data, update_time);
    
    if (copy_to_user(buffer, output, len))
        return -EFAULT;
        
    *pos = len;
    return len;
}

static ssize_t writer_proc(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    char input[16];
    int new_value;
    
    if (count > sizeof(input) - 1)
        return -EINVAL;
        
    if (copy_from_user(input, buffer, count))
        return -EFAULT;
        
    input[count] = '\0';
    
    if (kstrtoint(input, 10, &new_value) != 0)
        return -EINVAL;
    
    write_lock(&shared_rwlock);
    shared_data = new_value;
    last_update = jiffies;
    write_unlock(&shared_rwlock);
    
    pr_info("Updated shared_data to %d\n", new_value);
    return count;
}

static const struct proc_ops reader_fops = {
    .proc_read = reader_proc,
};

static const struct proc_ops writer_fops = {
    .proc_write = writer_proc,
};

static int __init rwlock_lab_init(void)
{
    proc_reader = proc_create(PROC_READER_NAME, 0444, NULL, &reader_fops);
    proc_writer = proc_create(PROC_WRITER_NAME, 0222, NULL, &writer_fops);
    
    if (!proc_reader || !proc_writer)
    {
        if (proc_reader) proc_remove(proc_reader);
        if (proc_writer) proc_remove(proc_writer);
        return -ENOMEM;
    }
    
    last_update = jiffies;
    pr_info("RWLock demo loaded\n");
    return 0;
}

static void __exit rwlock_lab_exit(void)
{
    proc_remove(proc_reader);
    proc_remove(proc_writer);
    pr_info("RWLock demo unloaded\n");
}

module_init(rwlock_lab_init);
module_exit(rwlock_lab_exit);

MODULE_LICENSE("GPL");