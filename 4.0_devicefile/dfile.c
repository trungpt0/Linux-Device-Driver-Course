#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

dev_t dev_num = 0;
static struct class *dev_class = NULL;

/**
 * Module initialization function
*/
static int __init dfile_init(void)
{
    int ret;

    /* Register a range of device numbers. The major number will be chosen dynamically. */
    if ((ret = alloc_chrdev_region(&dev_num, 0, 1, "rootv_dev")) != 0) {
        pr_err("can not alloc device major\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

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
    unregister_chrdev_region(dev_num, 1);
    return -1;
}

/**
 * Module exit function
*/
static void __exit dfile_exit(void)
{
    /* Removes a device */
    device_destroy(dev_class, dev_num);
    /* Destroys a struct class structure */
    class_destroy(dev_class);
    /* Unregister a range of device numbers */
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(dfile_init);
module_exit(dfile_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("Creating a device file");
MODULE_VERSION("1.0");