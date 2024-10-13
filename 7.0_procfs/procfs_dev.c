#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#define READ_DATA _IOR('a', 'a', int *)
#define WRITE_DATA _IOW('a', 'b', int *)

static dev_t dev_num = 0; // Device number
static struct class *dev_class = NULL; // Device class
static struct cdev rootv_cdev; // Character device structure
static struct proc_dir_entry *parent;

int value = 0;
char str[5] = "proc";

static int device_open(struct inode *inode, struct file *file)
{
    pr_info("device - open\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    pr_info("device - release\n");
    return 0;
}

static ssize_t device_read(struct file *filp, char __user *buf, size_t count, loff_t *off)
{
    pr_info("device - read\n");
    return 0;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *off)
{
    pr_info("device - write\n");
    return count;
}

static long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd)
    {
    case READ_DATA:
        if (copy_to_user((int __user *)arg, &value, sizeof(value))) {
            pr_err("ioctl - read err\n");
            return -EFAULT;
        }
        break;
    case WRITE_DATA:
        if (copy_from_user(&value, (int __user *)arg, sizeof(value))) {
            pr_err("ioctl - write    err\n");
        }
        pr_info("value = %d", value);
        break;
    default:
        break;
    }
    return 0;
}

static struct file_operations fops =
{
    .open = &device_open,
    .release = &device_release,
    .read = &device_read,
    .write = &device_write,
    .unlocked_ioctl = &device_ioctl,
};

static int device_proc_open(struct inode *inode, struct file *file)
{
    pr_info("proc - open\n");
    return 0;
}

static int device_proc_release(struct inode *inode, struct file *file)
{
    pr_info("proc - release\n");
    return 0;
}

static ssize_t device_proc_read(struct file *filp, char __user *buf, size_t count, loff_t *off)
{
    if (*off != 0) {
        return 0; // End of file
    }

    if (copy_to_user(buf, str, sizeof(str))) {
        pr_err("proc - read err\n");
        return -EFAULT;
    }
    return sizeof(str);
}

static ssize_t device_proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *off)
{
    if (count > sizeof(str)) {
        count = sizeof(str); // Limit the count to buffer size
    }

    if (copy_from_user(str, buf, count)) {
        pr_err("proc - write err\n");
        return -EFAULT;
    }
    return count;
}

static struct proc_ops pops =
{
    .proc_open = &device_proc_open,
    .proc_release = &device_proc_release,
    .proc_read = &device_proc_read,
    .proc_write = &device_proc_write,
};

/**
 * Module initialization function
*/
static int __init proc_init(void)
{
    int ret;

    /* Register a range of device numbers. The major number will be chosen dynamically. */
    if ((ret = alloc_chrdev_region(&dev_num, 0, 1, "rootv_dev")) != 0) {
        pr_err("can not alloc device major\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    cdev_init(&rootv_cdev, &fops);
    if (cdev_add(&rootv_cdev, dev_num, 1) < 0) {
        pr_err("can not add character device\n");
        goto cdev_err;
    }

    /* Create a struct class structure */
    // dev_class = class_create(THIS_MODULE, "rootv_class"); // SBC
    dev_class = class_create("rootv_class"); // Host
    if (IS_ERR(dev_class)) {
        pr_err("can not create class device\n");
        goto class_err;
    }

    /* Creates a device */
    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "rootv_device"))) {
        pr_err("can not create device\n");
        goto dev_err;
    }

    parent = proc_mkdir("rootv", NULL);
    if (parent == NULL) {
            pr_info("create proc - failed\n");
            goto proc_err;
    }
    proc_create("rootv_proc", 0666, parent, &pops);

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

proc_err:
    device_destroy(dev_class, dev_num);
dev_err:
    class_destroy(dev_class);
class_err:
    cdev_del(&rootv_cdev);
cdev_err:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/**
 * Module exit function
*/
static void __exit proc_exit(void)
{
    proc_remove(parent);
    cdev_del(&rootv_cdev); // Remove the character device
    device_destroy(dev_class, dev_num); // Removes a device
    class_destroy(dev_class); // Destroys a struct class structure
    unregister_chrdev_region(dev_num, 1); // Unregister a range of device numbers
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(proc_init);
module_exit(proc_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A procfs device driver");
MODULE_VERSION("1.0");