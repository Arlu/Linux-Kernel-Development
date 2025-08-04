#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static struct kobject *lab03_kobj;
static int lab03_value = 0;

static ssize_t value_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", lab03_value);
}

static ssize_t value_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int ret = kstrtoint(buf, 10, &lab03_value);
    if (ret < 0)
        return ret;
    return count;
}

static struct kobj_attribute value_attribute = __ATTR_RW(value);

static int __init sysfs_lab_init(void)
{
    lab03_kobj = kobject_create_and_add("lab03_sysfs", kernel_kobj);
    if (!lab03_kobj)
        return -ENOMEM;
        
    if (sysfs_create_file(lab03_kobj, &value_attribute.attr))
    {
        kobject_put(lab03_kobj);
        return -ENOMEM;
    }
    
    pr_info("sysfs interface: /sys/kernel/lab03_sysfs/\n");
    return 0;
}

static void __exit sysfs_lab_exit(void)
{
    sysfs_remove_file(lab03_kobj, &value_attribute.attr);
    kobject_put(lab03_kobj);
    pr_info("sysfs module unloaded\n");
}

module_init(sysfs_lab_init);
module_exit(sysfs_lab_exit);

MODULE_LICENSE("GPL");