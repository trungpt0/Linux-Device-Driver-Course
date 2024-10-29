#include <linux/module.h>

/**
 * Module initialization function
*/
static int __init skeleton_init(void)
{
    pr_info("Hello World!\n");
    pr_info("Kernel Module Inserted Successfully!\n");
    return 0;
}

/**
 * Module exit function
*/
static void __exit skeleton_exit(void)
{
    pr_info("Goodbye!\n");
    pr_info("Kernel Module Removed Successfully!\n");
}

/**
 * Register initialization and exit functions of the module
*/
module_init(skeleton_init);
module_exit(skeleton_exit);

/**
 * Module Information
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("trung tran");
MODULE_DESCRIPTION("A skeleton device driver");
MODULE_VERSION("1.0");
