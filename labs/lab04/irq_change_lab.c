#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/string.h>

#define MAX_IRQS 256
static unsigned long prev_interrupts[MAX_IRQS];
static unsigned long curr_interrupts[MAX_IRQS];
static struct timer_list check_timer;
static int total_changes = 0;
static int last_check_changes = 0;
static struct proc_dir_entry *proc_entry;

static int parse_proc_interrupts(void)
{
    struct file *file;
    char *buffer;
    loff_t pos = 0;
    ssize_t ret;
    int changes = 0;
    char *line, *next_line;
    int irq;
    unsigned long count;
    
    file = filp_open("/proc/interrupts", O_RDONLY, 0);
    if (IS_ERR(file))
        return PTR_ERR(file);
    
    buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!buffer)
    {
        filp_close(file, NULL);
        return -ENOMEM;
    }
    
    /* Copy previous to compare */
    memcpy(prev_interrupts, curr_interrupts, sizeof(prev_interrupts));
    memset(curr_interrupts, 0, sizeof(curr_interrupts));
    
    ret = kernel_read(file, buffer, PAGE_SIZE - 1, &pos);
    if (ret > 0)
    {
        buffer[ret] = '\0';
        
        /* Parse each line */
        line = buffer;
        while (line && *line)
        {
            next_line = strchr(line, '\n');
            if (next_line)
            {
                *next_line = '\0';
                next_line++;
            }
            
            /* Try to parse IRQ number and first CPU count */
            if (sscanf(line, " %d: %lu", &irq, &count) == 2)
            {
                if (irq >= 0 && irq < MAX_IRQS)
                {
                    curr_interrupts[irq] = count;
                    if (count != prev_interrupts[irq])
                    {
                        changes++;
                    }
                }
            }
            
            line = next_line;
        }
    }
    
    kfree(buffer);
    filp_close(file, NULL);
    return changes;
}

static void check_interrupts(struct timer_list *t)
{
    last_check_changes = parse_proc_interrupts();
    if (last_check_changes > 0)
    {
        total_changes += last_check_changes;
        pr_info("Interrupt changes detected: %d (total: %d)\n", 
                last_check_changes, total_changes);
    }
    
    mod_timer(&check_timer, jiffies + msecs_to_jiffies(5000));
}

static int irq_change_show(struct seq_file *m, void *v)
{
    int i, active_irqs = 0;
    
    seq_printf(m, "=== Interrupt Change Monitor ===\n\n");
    seq_printf(m, "Last check changes: %d\n", last_check_changes);
    seq_printf(m, "Total changes detected: %d\n", total_changes);
    seq_printf(m, "Monitoring interval: 5 seconds\n\n");
    
    seq_printf(m, "Active IRQs:\n");
    for (i = 0; i < MAX_IRQS; i++)
    {
        if (curr_interrupts[i] > 0)
        {
            seq_printf(m, "  IRQ %d: %lu", i, curr_interrupts[i]);
            if (curr_interrupts[i] != prev_interrupts[i])
                seq_printf(m, " (changed from %lu)", prev_interrupts[i]);
            seq_printf(m, "\n");
            active_irqs++;
        }
    }
    
    if (active_irqs == 0)
        seq_printf(m, "  No active IRQs found\n");
    
    return 0;
}

static int irq_change_open(struct inode *inode, struct file *file)
{
    return single_open(file, irq_change_show, NULL);
}

static const struct proc_ops irq_change_fops = {
    .proc_open = irq_change_open,
    .proc_read = seq_read,
    .proc_release = single_release,
};

static int __init irq_change_init(void)
{
    /* Initial parse to set baseline */
    parse_proc_interrupts();
    
    timer_setup(&check_timer, check_interrupts, 0);
    mod_timer(&check_timer, jiffies + msecs_to_jiffies(1000));
    
    proc_entry = proc_create("irq_changes", 0444, NULL, &irq_change_fops);
    if (!proc_entry)
    {
        timer_delete_sync(&check_timer);
        return -ENOMEM;
    }
    
    pr_info("IRQ change monitor loaded\n");
    return 0;
}

static void __exit irq_change_exit(void)
{
    timer_delete_sync(&check_timer);
    proc_remove(proc_entry);
    pr_info("IRQ change monitor unloaded: %d total changes\n", total_changes);
}

module_init(irq_change_init);
module_exit(irq_change_exit);

MODULE_LICENSE("GPL");