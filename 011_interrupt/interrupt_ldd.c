#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define GPIO_INTERRUPT_PIN 49

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;

static int irq_number;
static int count_irq = 0;

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
    count = snprintf(kbuffer, sizeof(kbuffer), "%d\n", count_irq);

    /* data enough sent */
    if (*off >= count) {
        return 0;
    }
    if (len < count) {
        count = len;
    }

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

/**
 * Interrupt handler
 */
static irqreturn_t irq_handler(int irq, void *dev)
{
    pr_info("interrupt!\n");
    count_irq++;
    return IRQ_HANDLED;
}

/* Add the device driver */
static int __init itr_driver_init(void)
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
    gpio_request(GPIO_INTERRUPT_PIN, "GPIO Interrupt Pin");
    gpio_direction_input(GPIO_INTERRUPT_PIN);
    irq_number = gpio_to_irq(GPIO_INTERRUPT_PIN);
    if (request_irq(irq_number, irq_handler, IRQF_TRIGGER_RISING, "rootv_dev", NULL)) {
        pr_err("cannot request irq\n");
        goto err_irq;
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_irq:
    if (irq_number) {
        free_irq(irq_number, NULL);
    }
err_cdev:
    device_destroy(dev_class, dev_num);
    cdev_del(&rootv_cdev);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/* Remove driver */
static void __exit itr_driver_exit(void)
{
    if (irq_number) {
        free_irq(irq_number, NULL);
    }
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(itr_driver_init);
module_exit(itr_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Interrupt Linux Driver");
MODULE_VERSION("1.0");
