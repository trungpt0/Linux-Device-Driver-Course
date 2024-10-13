#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/gpio.h>

#define BUFFER_SIZE 1024

dev_t dev_num = 0; // Device number
static struct class *dev_class = NULL; // Device class
static struct cdev rootv_cdev; // Character device structure

// Function to handle opening the device
static int device_open(struct inode *inode, struct file *filp)
{
    pr_info("RootV device was opened!\n");
    return 0;
}

// Function to handle closing the device
static int device_release(struct inode *inode, struct file *file)
{
    pr_info("RootV device was closed!\n");
    return 0;
}

// Function to handle reading from the device
static ssize_t device_read(struct file *filp, char __user *buffer, size_t count, loff_t *offset)
{
    pr_info("RootV device was read!\n");
    return BUFFER_SIZE;
}

// Function to handle writing to the device
static ssize_t device_write(struct file *filp, const char __user *buffer, size_t count, loff_t *offset)
{
    char value;

    if (copy_from_user(&value, buffer, sizeof(value))) {
        pr_err("write error!");
    }

    if (value == '1') {
        gpio_set_value(60, 1);
    } else {
        gpio_set_value(60, 0);
    }

    pr_info("RootV device was write!\n");
    return count;
}

static struct file_operations fops =
{
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
    .owner = THIS_MODULE,
};

/**
 * Module initialization function
*/
static int __init gpio_init(void)
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
    dev_class = class_create(THIS_MODULE, "rootv_class"); // BBB
    // dev_class = class_create("rootv_class"); // Host
    if (IS_ERR(dev_class)) {
        pr_err("can not create class device\n");
        goto class_err;
    }

    /* Creates a device */
    if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, "rootv_device"))) {
        pr_err("can not create device\n");
        goto dev_err;
    }

    /* Init GPIO 60 */
    if(gpio_request(60, "gpio_60_BBB")) {
        pr_err("can not alloc GPIO60");
        goto dev_err;
    }

    /* Set output direction for GPIO */
    if (gpio_direction_output(60, 0)) {
        pr_err("can not set direction for GPIO60");
        goto gpio_err;
    }

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;

gpio_err:
    gpio_free(60);
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
static void __exit gpio_exit(void)
{
    gpio_free(60);
    cdev_del(&rootv_cdev); // Remove the character device
    device_destroy(dev_class, dev_num); // Removes a device
    class_destroy(dev_class); // Destroys a struct class structure
    unregister_chrdev_region(dev_num, 1); // Unregister a range of device numbers
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(gpio_init);
module_exit(gpio_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("GPIO Driver");
MODULE_VERSION("1.0");