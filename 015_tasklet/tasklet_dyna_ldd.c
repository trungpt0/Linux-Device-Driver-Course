#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/slab.h>

#define INTERRUPT_LINE 30

dev_t dev_num;
static struct class *dev_class;
static struct cdev rootv_cdev;
struct tasklet_struct *tasklet;
int value = 0;

/* tasklet function */
void tasklet_fn(unsigned long data)
{
    pr_info("tasklet function with value: %d!\n", value);
}

/* irq handler */
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    pr_info("interrupted!\n");
    tasklet_schedule(tasklet);
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
    pr_info("device was read");
    return 0;
}

/* Write rootv device file */
static ssize_t rootv_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    char kbuffer[10];

    if (len > sizeof(kbuffer) - 1) len = sizeof(kbuffer) - 1;
    if (copy_from_user(kbuffer, buf, len)) return -EFAULT;

    kbuffer[len] = '\0';
    if (kstrtoint(kbuffer, 10, &value) < 0) return -EINVAL;

    tasklet_schedule(tasklet);

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

static int __init tasket_dyna_driver_init(void)
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

    /* ckeck gpio */
    if (!gpio_is_valid(INTERRUPT_LINE)) {
        pr_info("interrupt gpio is invalid!\n");
        goto err_gpio;
    }

    gpio_request(INTERRUPT_LINE, "GPIO_IRQ");
    gpio_direction_input(INTERRUPT_LINE);

    /* register irq */
    if (request_irq(gpio_to_irq(INTERRUPT_LINE), irq_handler, IRQF_TRIGGER_RISING, "rootv_devive", NULL)) {
        pr_info("cannot register irq!\n");
        goto err_irq;
    }

    tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
    if (!tasklet) {
        pr_info("cannot alloc tasklet!\n");
        goto err_tasklet;
    }
    tasklet_init(tasklet, tasklet_fn, 0);

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

err_tasklet:
    free_irq(gpio_to_irq(INTERRUPT_LINE), NULL);
err_irq:
    gpio_free(INTERRUPT_LINE);
err_gpio:
    cdev_del(&rootv_cdev);
err_cdev:
    device_destroy(dev_class, dev_num);
err_dev:
    class_destroy(dev_class);
err_class:
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

static void __exit tasket_dyna_driver_exit(void)
{
    tasklet_kill(tasklet);
    kfree(tasklet);
    free_irq(gpio_to_irq(INTERRUPT_LINE), NULL);
    gpio_free(INTERRUPT_LINE);
    cdev_del(&rootv_cdev);
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    unregister_chrdev_region(dev_num, 1)    ;
    pr_info("Kernel Module Removed Successfully\n");
}

/**
 * Register initialization and exit function of the module
 */
module_init(tasket_dyna_driver_init);
module_exit(tasket_dyna_driver_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trung Tran");
MODULE_DESCRIPTION("Tasket Dynamic Linux Driver");