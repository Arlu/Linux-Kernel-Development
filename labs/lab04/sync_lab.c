#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/rwlock.h>
#include <linux/kthread.h>
#include <linux/delay.h>
// #include <linux/uaccess.h>
#include <linux/seq_file.h>

static int shared_counter = 0;
static DEFINE_SPINLOCK(counter_lock);
static DEFINE_MUTEX(data_mutex);
static DEFINE_RWLOCK(config_lock);

static int config_value = 100;
static struct task_struct *worker_thread;
static struct proc_dir_entry *proc_entry;

static int worker_function(void *data)
{
    int i;
    
    for (i = 0; i < 1000 && !kthread_should_stop(); i++)
    {
        /* Test spinlock */
        spin_lock(&counter_lock);
        shared_counter++;
        spin_unlock(&counter_lock);
        
        /* Test rwlock read */
        read_lock(&config_lock);
        if (config_value > 0)
            pr_info("Config value: %d\n", config_value);
        read_unlock(&config_lock);
        
        msleep(10);
    }

    return 0;
}

static int sync_show(struct seq_file *m, void *v)
{
    read_lock(&config_lock);
    seq_printf(m, "=== Synchronization Lab ===\n");
    seq_printf(m, "Counter: %d\n", shared_counter);
    seq_printf(m, "Config: %d\n", config_value);
    seq_printf(m, "Worker thread: %s\n", worker_thread ? "Running" : "Stopped");
    read_unlock(&config_lock);
    return 0;
}

static ssize_t sync_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    int new_value;
    char input[32];
    
    if (count >= sizeof(input))
        return -EINVAL;
    
    if (copy_from_user(input, buffer, count))
        return -EFAULT;
    
    input[count] = '\0';
    
    if (sscanf(input, "%d", &new_value) == 1)
    {
        write_lock(&config_lock);
        config_value = new_value;
        write_unlock(&config_lock);
        pr_info("Config updated to: %d\n", new_value);
    }
    
    return count;
}

static int sync_open(struct inode *inode, struct file *file)
{
    return single_open(file, sync_show, NULL);
}

static const struct proc_ops sync_fops = {
    .proc_open = sync_open,
    .proc_read = seq_read,
    .proc_write = sync_write,
    .proc_release = single_release,
};

static int __init sync_lab_init(void)
{
    proc_entry = proc_create("sync_lab", 0666, NULL, &sync_fops);
    if (!proc_entry)
        return -ENOMEM;
    
    worker_thread = kthread_run(worker_function, NULL, "sync_worker");
    if (IS_ERR(worker_thread))
    {
        proc_remove(proc_entry);
        return PTR_ERR(worker_thread);
    }
    
    pr_info("Sync lab loaded\n");
    return 0;
}

static void __exit sync_lab_exit(void)
{
    if (worker_thread)
        kthread_stop(worker_thread);
    
    proc_remove(proc_entry);
    pr_info("Sync lab unloaded: counter=%d\n", shared_counter);
}

module_init(sync_lab_init);
module_exit(sync_lab_exit);

MODULE_LICENSE("GPL");