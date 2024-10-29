#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kobject.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;
static struct kobject *kobj;

int sysfs_value = 0;

/* Open rootv device file */
static int rootv_open(struct inode *inode, struct file *file) {
    pr_info("device opened\n");
    return 0;
}

/* Close rootv device file */
static int rootv_release(struct inode *inode, struct file *file) {
    pr_info("device closed\n");
    return 0;
}

/* Read rootv device file */
static ssize_t rootv_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    char temp_buffer[10];
    int data_len = sprintf(temp_buffer, "%d\n", sysfs_value); // get value from sysfs value

    if (*off >= data_len) {
        return 0;
    }
    /* send data to user */
    if (copy_to_user(buf, temp_buffer, data_len)) {
        pr_err("cannot read device file data\n");
        return -EFAULT;
    }
    *off += data_len;

    pr_info("device was read\n");
    return data_len;
}

/* Write rootv device file */
static ssize_t rootv_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    char *kbuf = kmalloc(len + 1, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;
    
    /* write data from defile file to sysfs device */
    if (copy_from_user(kbuf, buf, len)) {
        pr_err("cannot write to device file\n");
        kfree(kbuf);
        return -EFAULT;
    }
    kbuf[len] = '\0';
    kstrtoint(kbuf, 10, &sysfs_value);
    kfree(kbuf);
    
    pr_info("device was wrote\n");
    return len;
}

/* File operations device file */
static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = rootv_open,
    .release = rootv_release,
    .read = rootv_read,
    .write = rootv_write,
};

/* Read value from sysfs device */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    pr_info("sysfs file was read\n");
    return sprintf(buf, "%d", sysfs_value);
}

/* Write value to sysfs device */
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    pr_info("sysfs file was write\n");
    sscanf(buf, "%d", &sysfs_value);
    return count;
}

static struct kobj_attribute rootv_attr = __ATTR(value, 0660, sysfs_show, sysfs_store);

/* Add the device driver */
static int __init sysfs_driver_init(void)
{
    /* Resgister device number */
    if ((alloc_chrdev_region(&dev_num, 0, 1, "rootv_device")) < 0) {
        pr_err("cannot alloc device number\n");
        return -1;
    }
    pr_info("rootv_device - major %d minor %d\n", MAJOR(dev_num), MINOR(dev_num));

    /* Create class device */
    if (IS_ERR(dev_class = class_create(THIS_MODULE, "rootv_class"))) {
        pr_err("cannot create device class\n");
        goto err_class;
    }

    /* Create device */
    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "rootv_dev"))) {
        pr_err("cannot create device\n");
        goto err_dev;
    }

    /* Add device to the system */
    cdev_init(&rootv_cdev, &fops);
    if (cdev_add(&rootv_cdev, dev_num, 1) < 0) {
        pr_err("cannot add deivce\n");
        goto err_cdev;
    }

    /* Add device into /sys */
    kobj = kobject_create_and_add("rootv_sysfs", kernel_kobj);
    if (!kobj) {
        pr_err("cannot create sysfs object\n");
        goto err_kobj;
    }
    
    /* Init value in sysfs device */
    if (sysfs_create_file(kobj, &rootv_attr.attr)) {
        pr_err("cannot create sysfs file\n");
        goto err_sysfs;
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_sysfs:
    kobject_put(kobj);
    sysfs_remove_file(kobj, &rootv_attr.attr);
err_kobj:
    cdev_del(&rootv_cdev);
err_cdev:
    device_destroy(dev_class, dev_num);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num,1);
    return -1;
}

/* Remove driver */
static void __exit sysfs_driver_exit(void)
{
    kobject_put(kobj);
    sysfs_remove_file(kobj, &rootv_attr.attr);
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num,1);
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(sysfs_driver_init);
module_exit(sysfs_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Sysfs Linux Driver");
MODULE_VERSION("1.0");
