#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define READ_IOCTL _IOR('a', 'a', int[10])
#define WRITE_IOCTL _IOW('a', 'b', int[10])
#define RW_IOCTL _IOWR('a', 'c', int)

static dev_t dev_num = 0; // Device number
static struct class *dev_class = NULL; // Device class
static struct cdev rootv_cdev; // Character device structure
static int arr[10] = {0};

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
    int n;

    switch (cmd)
    {
    case READ_IOCTL:
        if (copy_to_user((int __user *)arg, arr, sizeof(arr))) {
            pr_err("read failed\n");
            return -EFAULT;
        }
        break;
    case WRITE_IOCTL:
        if (copy_from_user(arr, (int __user *)arg, sizeof(arr))) {
            pr_err("write failed\n");
            return -EFAULT;
        }
        break;
    case RW_IOCTL:
        if (copy_from_user(&n, (int __user *)arg, sizeof(n))) {
            pr_err("failed to get factor from user\n");
            return -EFAULT;
        }
        for (int i = 0; i < sizeof(arr) / sizeof(int); i++) {
            arr[i] *= n;
        }
        if (copy_to_user((int __user *)arg, &n, sizeof(n))) {
            pr_err("failed to send result to user\n");
            return -EFAULT;
        }
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

/**
 * Module initialization function
*/
static int __init ioctl_init(void)
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

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

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
static void __exit ioctl_exit(void)
{
    cdev_del(&rootv_cdev); // Remove the character device
    device_destroy(dev_class, dev_num); // Removes a device
    class_destroy(dev_class); // Destroys a struct class structure
    unregister_chrdev_region(dev_num, 1); // Unregister a range of device numbers
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(ioctl_init);
module_exit(ioctl_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A ioctl device driver");
MODULE_VERSION("1.0");