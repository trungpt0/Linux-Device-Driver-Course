#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>

DECLARE_WAIT_QUEUE_HEAD(waitqueue_dev);

static dev_t dev_num = 0; // Device number
static struct class *dev_class = NULL; // Device class
static struct cdev rootv_cdev; // Character device structure
static struct task_struct *thread1;
int flag = 0;
int count = 0;

static int thread_handle(void *data)
{
    while (!kthread_should_stop())
    {
        pr_info("thread: waiting for an event...\n");
        wait_event_interruptible(waitqueue_dev, flag != 0);
        if (flag == 2) {
            pr_info("thread: event from exit function\n");
            return 0;
        }
        pr_info("thread: event from read function count = %d\n", ++count);
        flag = 0;
    }
    return 0;
}

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
    flag = 1;
    wake_up_interruptible(&waitqueue_dev);
    return 0;
}

static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *off)
{
    pr_info("device - write\n");
    return count;
}

static struct file_operations fops =
{
    .open = &device_open,
    .release = &device_release,
    .read = &device_read,
    .write = &device_write,
};

/**
 * Module initialization function
*/
static int __init waitqueue_init(void)
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

    // Initialize and start the kernel thread
    init_waitqueue_head(&waitqueue_dev);
    thread1 = kthread_run(thread_handle, NULL, "wait_thread");
    if (IS_ERR(thread1)) {
        pr_err("Failed to create kernel thread\n");
        ret = PTR_ERR(thread1);
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
static void __exit waitqueue_exit(void)
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
module_init(waitqueue_init);
module_exit(waitqueue_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A waitqueue device driver");
MODULE_VERSION("1.0");