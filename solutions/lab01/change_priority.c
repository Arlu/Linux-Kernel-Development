/**
 * change_priority.c - Kernel module to change priority of a specific process
 *
 * This module monitors a specific process identified by its PID
 * and prints detailed information about it to the kernel log.
 *
 * Usage: insmod change_priority.ko
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define PROCFS_NAME "process_priority"

static struct proc_dir_entry *proc_file;

static ssize_t proc_write(struct file *f, const char __user *buf, size_t count, loff_t *ppos)
{
    struct task_struct *task;
    int pid, priority;

    if (sscanf(buf, "%d %d", &pid, &priority) != 2) {
        pr_warn("Invalid input format. Expected: <pid - int> <priority - int>\n");
        return -EINVAL;
    }

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task)
    {
        pr_err("Process with PID %d not found\n", pid);
        return -ESRCH;
    }

    task->prio = priority;

    pr_info("Setting pid = %d with priority = %d\n", pid, priority);

    return count;
}

static const struct proc_ops proc_fops = {
    .proc_write = proc_write
};

static int __init change_priority_init(void)
{
    /* Create proc entry */
    proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_fops);
    if (proc_file == NULL) {
        pr_err("Failed to create /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }
    
    pr_info("Change priority module loaded. For change process priority:\n");
    pr_info("Call as: echo \"<pid - int> <priority - int>\" > /proc/%s\n", PROCFS_NAME);
    return 0;
}

static void __exit change_priority_exit(void)
{
    proc_remove(proc_file);
    pr_info("Change priority module unloaded\n");
}

module_init(change_priority_init);
module_exit(change_priority_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arie");
MODULE_DESCRIPTION("Change priority module");
MODULE_VERSION("1.0");