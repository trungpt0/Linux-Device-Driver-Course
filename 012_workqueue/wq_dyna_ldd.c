#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>

#define GPIO_INTERRUPT_PIN 49

/* Workqueue dynamic */
static struct workqueue_struct *workqueue;
static struct work_struct work;

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;

static int irq_number;
static atomic_t count_irq = ATOMIC_INIT(0); // atomic variable used to access count_irq one time one thread

void workqueue_fn(struct work_struct *work)
{
    pr_info("in workqueue function\n");
}

/**
 * Interrupt handler
 */
static irqreturn_t irq_handler(int irq, void *dev)
{
    pr_info("interrupt!\n");
    atomic_inc(&count_irq); // Use atomic increment for safe concurrent access
    
    /* Onlu schedule work if no other instance is already pending */
    if (!work_pending(&work)) {
        queue_work(workqueue, &work);
    }

    return IRQ_HANDLED;
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
    char kbuffer[10]; // kernel buffer
    int count; // interrupt count

    /* int -> char */
    count = snprintf(kbuffer, sizeof(kbuffer), "%d\n", atomic_read(&count_irq));

    /* check if all data has been sent */
    if (*off >= count) return 0;
    if (len < count) count = len;

    /* copy data from kernel -> user */
    if (copy_to_user(buf, kbuffer, count)) {
        pr_err("cannot copy data to user\n");
        return -EFAULT;
    }

    /* update offset */
    *off += count;

    pr_info("device was read");
    return count;
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

/* Add the device driver */
static int __init wq_driver_init(void)
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
    
    /* Register irq */
    if (gpio_request(GPIO_INTERRUPT_PIN, "GPIO Interrupt Pin") < 0) goto err_gpio;
    gpio_direction_input(GPIO_INTERRUPT_PIN);
    irq_number = gpio_to_irq(GPIO_INTERRUPT_PIN);
    if (request_irq(irq_number, irq_handler, IRQF_TRIGGER_RISING, "rootv_dev", NULL)) {
        pr_err("cannot request irq\n");
        goto err_irq;
    }

    workqueue = create_workqueue("my_workqueue");
    if (!workqueue) goto err_wq;
    INIT_WORK(&work, workqueue_fn);

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_wq:
    free_irq(irq_number, NULL);
err_irq:
    gpio_free(GPIO_INTERRUPT_PIN);
err_gpio:
    device_destroy(dev_class, dev_num);
err_cdev:
    cdev_del(&rootv_cdev);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/* Remove driver */
static void __exit wq_driver_exit(void)
{
    if (workqueue) {
        flush_workqueue(workqueue);
        destroy_workqueue(workqueue);
    }
    if (irq_number) free_irq(irq_number, NULL);
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(wq_driver_init);
module_exit(wq_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Workqueue Linux Driver");
MODULE_VERSION("1.0");
