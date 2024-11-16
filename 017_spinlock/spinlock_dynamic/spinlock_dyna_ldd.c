#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/string.h>

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;

unsigned int value = 0;
struct task_struct *k_thread1;
struct task_struct *k_thread2;

spinlock_t spinlock;

static int thread_fn1(void *)
{
    while (!kthread_should_stop()) {
        if (!spin_is_locked(&spinlock)) {
            pr_info("spinlock is unlocked!\n");
        }
        spin_lock(&spinlock);
        if (spin_is_locked(&spinlock)) {
            pr_info("spinlock is locked!\n");
        }
        value++;
        pr_info("thread1, value = %d\n", value);
        spin_unlock(&spinlock);
        msleep(1000);
    }
    return 0;
}

static int thread_fn2(void *)
{
    while (!kthread_should_stop()) {
        spin_lock(&spinlock);
        value++;
        pr_info("thread2, value = %d\n", value);
        spin_unlock(&spinlock);
        msleep(1000);
    }
    return 0;
}

static int rootv_open(struct inode *inode, struct file *file)
{
    pr_info("device was opened!\n");
    return 0;
}

static int rootv_release(struct inode *inode, struct file *file)
{
    pr_info("device was closed!\n");
    return 0;
}

static ssize_t rootv_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    char kbuffer[10];
    int t_value;
    
    spin_lock(&spinlock);
    t_value = value;
    spin_unlock(&spinlock);

    snprintf(kbuffer, sizeof(kbuffer), "value: %d\n", t_value);

    if (*off >= strlen(kbuffer)) return 0;
    if (len > strlen(kbuffer) - *off) len = strlen(kbuffer) - *off;

    if (copy_to_user(buf, kbuffer, strlen(kbuffer) + 1)) {
        return -EFAULT;
    }
    
    *off += strlen(kbuffer) + 1;

    pr_info("device was read!\n");
    return strlen(kbuffer) + 1;
}

static ssize_t rootv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("device was wrote!\n");
    return len;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = rootv_open,
    .release = rootv_release,
    .read = rootv_read,
    .write = rootv_write,   
};

static int __init spinlock_driver_init(void)
{
    if (alloc_chrdev_region(&dev_num, 0, 1, "rootv_device") < 0) {
        pr_err("cannot alloc device number\n");
        return -1;
    }
    pr_info("rootv_device - major %d minor %d\n", MAJOR(dev_num), MINOR(dev_num));

    if (IS_ERR(dev_class = class_create(THIS_MODULE, "rootv_class"))) {
        pr_err("cannot create device class\n");
        goto err_class;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "rootv_dev"))) {
        pr_err("cannot create device\n");
        goto err_dev;
    }

    cdev_init(&rootv_cdev, &fops);
    if (cdev_add(&rootv_cdev, dev_num, 1) < 0) {
        pr_err("cannot add device\n");
        goto err_cdev;
    }

    spin_lock_init(&spinlock);

    k_thread1 = kthread_run(thread_fn1, NULL, "rootv-thread1");
    if (k_thread1) {
        pr_err("thread1 created successfully\n");
    } else {
        pr_err("cannot create thread1\n");
    }

    k_thread2 = kthread_run(thread_fn2, NULL, "rootv-thread2");
    if (k_thread2) {
        pr_err("thread2 created successfully\n");
    } else {
        pr_err("cannot create thread2\n");
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_cdev:
    device_destroy(dev_class, dev_num);
    cdev_del(&rootv_cdev);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit spinlock_driver_exit(void)
{
    kthread_stop(k_thread1);
    kthread_stop(k_thread2);
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(spinlock_driver_init);
module_exit(spinlock_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Spinlock Static Linux Driver");