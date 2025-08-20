#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/list.h>
#include <linux/hashtable.h>
#include <linux/kfifo.h>
// #include <linux/slab.h>
// #include <linux/uaccess.h>

/* Task list */
struct task_entry
{
    int id;
    char name[32];
    int priority;
    struct list_head list;
};

static LIST_HEAD(task_list);

/* Hash table for users */
#define USER_HASH_BITS 4
struct user_entry
{
    int uid;
    char name[32];
    struct hlist_node node;
};

static DEFINE_HASHTABLE(user_table, USER_HASH_BITS);

/* Message queue */
struct message
{
    int type;
    char data[64];
};

static DEFINE_KFIFO(msg_queue, struct message, 16);

static struct proc_dir_entry *proc_entry;

static int data_show(struct seq_file *m, void *v)
{
    struct task_entry *task;
    struct user_entry *user;
    struct message msg;
    int bkt;
    
    seq_printf(m, "=== Data Structures Lab ===\n\n");
    
    seq_printf(m, "Tasks:\n");
    list_for_each_entry(task, &task_list, list)
    {
        seq_printf(m, "  ID: %d Name: %s Priority: %d\n", 
                   task->id, task->name, task->priority);
    }
    
    seq_printf(m, "\nUsers:\n");
    hash_for_each(user_table, bkt, user, node)
    {
        seq_printf(m, "  UID: %d Name: %s\n", user->uid, user->name);
    }
    
    seq_printf(m, "\nMessage Queue:\n");
    seq_printf(m, "  Messages: %u/%u\n", kfifo_len(&msg_queue), kfifo_size(&msg_queue));
    
    /* Show a few messages */
    while (kfifo_out_peek(&msg_queue, &msg, 1))
    {
        seq_printf(m, "  Type: %d Data: %s\n", msg.type, msg.data);
        break; /* Just show first message */
    }
    
    return 0;
}

static ssize_t data_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
    char input[128];
    char cmd[16], name[32];
    int id, priority;
    
    if (count >= sizeof(input))
        return -EINVAL;
    
    if (copy_from_user(input, buffer, count))
        return -EFAULT;
    
    input[count] = '\0';
    
    if (sscanf(input, "%s %d %s %d", cmd, &id, name, &priority) == 4)
    {
        if (strcmp(cmd, "task") == 0)
        {
            struct task_entry *task = kmalloc(sizeof(*task), GFP_KERNEL);
            if (task)
            {
                task->id = id;
                task->priority = priority;
                strncpy(task->name, name, sizeof(task->name) - 1);
                task->name[sizeof(task->name) - 1] = '\0';
                list_add_tail(&task->list, &task_list);
                pr_info("Added task: %s\n", name);
            }
        }
        else if
        (strcmp(cmd, "user") == 0)
        {
            struct user_entry *user = kmalloc(sizeof(*user), GFP_KERNEL);
            if (user)
            {
                user->uid = id;
                strncpy(user->name, name, sizeof(user->name) - 1);
                user->name[sizeof(user->name) - 1] = '\0';
                hash_add(user_table, &user->node, id);
                pr_info("Added user: %s\n", name);
            }
        }
        else if (strcmp(cmd, "msg") == 0)
        {
            struct message msg;
            msg.type = id;
            strncpy(msg.data, name, sizeof(msg.data) - 1);
            msg.data[sizeof(msg.data) - 1] = '\0';
            if (kfifo_in(&msg_queue, &msg, 1))
                pr_info("Added message: %s\n", name);
        }
    }
    
    return count;
}

static int data_open(struct inode *inode, struct file *file)
{
    return single_open(file, data_show, NULL);
}

static const struct proc_ops data_fops = {
    .proc_open = data_open,
    .proc_read = seq_read,
    .proc_write = data_write,
    .proc_release = single_release,
};

static int __init data_lab_init(void)
{
    proc_entry = proc_create("data_lab", 0666, NULL, &data_fops);
    if (!proc_entry)
        return -ENOMEM;
    
    pr_info("Data structures lab loaded\n");
    return 0;
}

static void __exit data_lab_exit(void)
{
    struct task_entry *task, *tmp_task;
    struct user_entry *user;
    struct hlist_node *tmp;
    int bkt;
    
    list_for_each_entry_safe(task, tmp_task, &task_list, list)
    {
        list_del(&task->list);
        kfree(task);
    }
    
    hash_for_each_safe(user_table, bkt, tmp, user, node)
    {
        hash_del(&user->node);
        kfree(user);
    }
    
    proc_remove(proc_entry);
    pr_info("Data structures lab unloaded\n");
}

module_init(data_lab_init);
module_exit(data_lab_exit);

MODULE_LICENSE("GPL");