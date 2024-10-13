#include <linux/module.h>
#include <linux/fs.h>

dev_t dev_num;

/**
 * Module initialization function
*/
static int __init chardd_dyna_init(void)
{
    int ret;

    /* Register a range of device numbers. The major number will be chosen dynamically. */
    if ((ret = alloc_chrdev_region(&dev_num, 0, 1, "rootv_dev")) != 0) {
        pr_err("can not alloc device major\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;
}

/**
 * Module exit function
*/
static void __exit chardd_dyna_exit(void)
{
    /* Unregister a range of device numbers */
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(chardd_dyna_init);
module_exit(chardd_dyna_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A alloc device file static");
MODULE_VERSION("1.0");