#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;

static struct task_struct *rootv_thread;
static int count = 0;

/* thread function */
static int thread_fn(void *)
{
    while (!kthread_should_stop()) {
        count++;
        pr_info("thread running: count = %d\n", count);
        msleep(1000);
    }
    return 0;
}

/* Open rootv device file */
static int rootv_open(struct inode *inode, struct file *file)
{
    pr_info("device was opened\n");
    return 0;
}

/* Close rootv device file */
static int rootv_release(struct inode *inode, struct file *file)
{
    pr_info("device was closed\n");
    return 0;
}

/* Read rootv device file */
static ssize_t rootv_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    char kbuffer[16];
    int kcount = count;
    
    /* store data to kernel buffer */
    snprintf(kbuffer, sizeof(kbuffer), "%d\n", kcount);

    if (*off >= strlen(kbuffer)) return 0;
    if (len > strlen(kbuffer) - *off) len = strlen(kbuffer) - *off;

    if (copy_to_user(buf, kbuffer, len)) {
        pr_err("cannot copy data to user\n");
        return -EFAULT;
    }

    *off += len;

    pr_info("device was read");
    return len;
}

/* Write rootv device file */
static ssize_t rootv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("device was wrote");
    return len;
}

/* File operations device file */
static const struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = rootv_open,
    .release = rootv_release,
    .read = rootv_read,
    .write = rootv_write,
};

static int __init kthread_driver_init(void)
{
    /* Resgister device number */
    if (alloc_chrdev_region(&dev_num, 0, 1, "rootv_device") < 0) {
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
        pr_err("cannot add device\n");
        goto err_cdev;
    }

    rootv_thread = kthread_create(thread_fn, NULL, "rootv_thread");
    if (rootv_thread) {
        wake_up_process(rootv_thread);
        pr_info("thread was created\n");
    } else {
        pr_info("cannot create thread\n");
        goto err_thr;
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_thr:
    cdev_del(&rootv_cdev);
err_cdev:
    device_destroy(dev_class, dev_num);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit kthread_driver_exit(void)
{
    kthread_stop(rootv_thread);
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(kthread_driver_init);
module_exit(kthread_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Kernel Thread Linux Driver");
MODULE_VERSION("1.0");