#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/atomic.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/preempt.h>

static atomic_t counter = ATOMIC_INIT(0);

#define PROC_NAME "log_example"
static struct proc_dir_entry *proc_entry;

#define MAX_SIZE 128

struct log_entry
{
	int id;
	char value[64];
	struct list_head node;
	struct rcu_head rcu;
};

static LIST_HEAD(log);
static spinlock_t log_lock;

static void get_entry_callback(struct rcu_head *rcu)
{
	struct log_entry *en = container_of(rcu, struct log_entry, rcu);
	pr_info("callback free : %lx, preempt_count : %d\n", (unsigned long)en, preempt_count());
	kfree(en);
}

static ssize_t add_entry(const char *entry)
{
    struct log_entry *en;

	en = kzalloc(sizeof(struct log_entry), GFP_KERNEL);
	if(!en)
		return -EFAULT;

	en->id = atomic_read(&counter);
	strncpy(en->value, entry, sizeof(en->value));
    
    atomic_inc(&counter);

	spin_lock(&log_lock);
	list_add_rcu(&en->node, &log);
	spin_unlock(&log_lock);

    return 0;
}

static ssize_t delete_entry(int id, int async)
{
	struct log_entry *en;

	spin_lock(&log_lock);
	list_for_each_entry(en, &log, node)
    {
		if(en->id == id)
        {
			list_del_rcu(&en->node);
			spin_unlock(&log_lock);

			if (async)
            {
				call_rcu(&en->rcu, get_entry_callback);
			}
            else
            {
				synchronize_rcu();
				kfree(en);
			}
			return 0;
		}
	}
	spin_unlock(&log_lock);

	pr_err("The log entry isn't exist\n");
    return -EFAULT;
}

static ssize_t print_entry(int id)
{
	struct log_entry *en;

	rcu_read_lock();
	list_for_each_entry_rcu(en, &log, node)
    {
		if(en->id == id)
        {
			pr_info("Log entry id - %d: %s\n", en->id, en->value);
			rcu_read_unlock();
			return 0;
		}
	}
	rcu_read_unlock();

	pr_err("The log entry isn't exist\n");
    return -EFAULT;
}

static ssize_t proc_read(struct file *file, char __user *buffer, size_t count, loff_t *pos)
{
    int line_len, total_len;
    char kernel_buffer[MAX_SIZE];
    struct log_entry *en;
    
	rcu_read_lock();
    
    if (*pos > 0)
    {
        rcu_read_unlock();
        return 0;
    }

	list_for_each_entry_rcu(en, &log, node)
    {
        line_len += snprintf(kernel_buffer, MAX_SIZE, "Log entry id - %d: %s\n", en->id, en->value);
        if (copy_to_user(buffer + total_len, kernel_buffer, line_len))
        {
			rcu_read_unlock();
            return -EFAULT;
        }
        total_len += line_len;
	}
    rcu_read_unlock();

    *pos = total_len;
    return total_len;
}

static ssize_t proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    char input[68], cmd[4], log_value[64];
    int log_id, rc;
    
    if (count > sizeof(input) - 1)
        return -EINVAL;
        
    if (copy_from_user(input, buffer, count))
        return -EFAULT;
        
    input[count] = '\0';
    
	strncpy(cmd, input, sizeof(cmd));
    cmd[sizeof(cmd) - 1] = '\0';
	strncpy(log_value, input + 4, sizeof(log_value));
    log_value[strlen(log_value) - 1] = '\0';

    rc = kstrtoint(log_value, 10, &log_id);
    
    if (strcmp(cmd, "add") == 0)
    {
        add_entry(log_value);
    }
    else if (strcmp(cmd, "get") == 0)
    {
        print_entry(log_id);
    }
    else if (strcmp(cmd, "del") == 0)
    {
        delete_entry(log_id, 0);
    }
    
    return count;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init rcu_example_init(void)
{
	spin_lock_init(&log_lock);
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_fops);
    if (!proc_entry)
        return -ENOMEM;
        
    pr_info("RCU module loaded: /proc/%s\n", PROC_NAME);
    pr_info("  echo \"add <message>\" > /proc/%s\n", PROC_NAME);
    pr_info("  echo \"get <id>\" > /proc/%s\n", PROC_NAME);
    pr_info("  echo \"del <id>\" > /proc/%s\n", PROC_NAME);
    pr_info("  cat /proc/%s\n", PROC_NAME);
    return 0;
}

static void __exit rcu_example_exit(void)
{
    proc_remove(proc_entry);
    pr_info("RCU module unloaded\n");
}

module_init(rcu_example_init);
module_exit(rcu_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arie");
MODULE_DESCRIPTION("A example of rcu module");
MODULE_VERSION("1.0");