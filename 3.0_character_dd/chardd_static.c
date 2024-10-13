#include <linux/module.h>
#include <linux/fs.h>

dev_t dev_num = MKDEV(236, 0);

/**
 * Module initialization function
*/
static int __init chardd_static_init(void)
{
    int ret;

    /* Register a range of device numbers */
    if ((ret = register_chrdev_region(dev_num, 1, "rootv_dev")) != 0) {
        pr_err("can not register device\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(dev_num), MINOR(dev_num));

    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;
}

/**
 * Module exit function
*/
static void __exit chardd_static_exit(void)
{
    /* Unregister a range of device numbers */
    unregister_chrdev_region(dev_num, 1);
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(chardd_static_init);
module_exit(chardd_static_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A alloc device file static");
MODULE_VERSION("1.0");